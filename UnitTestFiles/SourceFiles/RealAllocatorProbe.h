/* Instrument the actual Eco allocator object, forwarding every successful
 * operation to its original implementation. Neither bus nor factory is mocked. */
#define PROBE_SLOTS 8192u
typedef struct RealMemoryProbe {
    IEcoMemoryAllocator1* memory;
    IEcoMemoryAllocator1VTbl* original;
    IEcoMemoryAllocator1VTbl wrapped;
    void* blocks[PROBE_SLOTS];
    uint32_t live, allocations, frees, calls, fail_at, invalid, enabled;
} RealMemoryProbe;
static RealMemoryProbe real_memory;
static voidptr_t ECOCALLMETHOD probe_alloc(IEcoMemoryAllocator1* me, uint32_t size) {
    uint32_t i; void* value;
    if (!real_memory.enabled) return real_memory.original->Alloc(me, size);
    ++real_memory.calls;
    if (real_memory.fail_at != 0u && real_memory.calls == real_memory.fail_at) return 0;
    value = real_memory.original->Alloc(me, size);
    if (value == 0) return 0;
    for (i = 0u; i < PROBE_SLOTS; ++i) if (real_memory.blocks[i] == 0) break;
    if (i == PROBE_SLOTS) { ++real_memory.invalid; real_memory.original->Free(me, value); return 0; }
    real_memory.blocks[i] = value; ++real_memory.live; ++real_memory.allocations;
    return value;
}
static void ECOCALLMETHOD probe_free(IEcoMemoryAllocator1* me, voidptr_t value) {
    uint32_t i;
    if (value == 0) return;
    for (i = 0u; i < PROBE_SLOTS; ++i) if (real_memory.blocks[i] == value) break;
    if (i < PROBE_SLOTS) {
        real_memory.blocks[i] = 0; --real_memory.live; ++real_memory.frees;
    }
    /* A framework object may have been allocated before instrumentation. */
    real_memory.original->Free(me, value);
}
static uint32_t probe_reference_count(void) {
    uint32_t count;
    count = real_memory.memory->pVTbl->AddRef(real_memory.memory);
    real_memory.memory->pVTbl->Release(real_memory.memory);
    return count - 1u;
}
static void probe_start(IEcoMemoryAllocator1* memory) {
    uint32_t i;
    real_memory.memory = memory; real_memory.original = memory->pVTbl;
    real_memory.wrapped = *memory->pVTbl;
    real_memory.wrapped.Alloc = probe_alloc; real_memory.wrapped.Free = probe_free;
    real_memory.live = 0u; real_memory.allocations = 0u; real_memory.frees = 0u;
    real_memory.enabled = 1u; real_memory.calls = 0u; real_memory.fail_at = 0u; real_memory.invalid = 0u;
    for (i = 0u; i < PROBE_SLOTS; ++i) real_memory.blocks[i] = 0;
    memory->pVTbl = &real_memory.wrapped;
}
static void probe_stop(void) {
    real_memory.memory->pVTbl = real_memory.original;
}
