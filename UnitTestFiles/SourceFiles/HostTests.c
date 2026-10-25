/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#include <stdio.h>
#include <stdlib.h>
#include "polyfit_component.h"
#include "IEcoPolynomial.h"
#include "IdEcoPolynomialMath.h"
#include "polyfit_bus.h"

/* Explicit host-only allocator double. Each allocation is a real malloc block,
 * so ASan observes actual object boundaries and use-after-free. */
#define TEST_SLOTS 8192u
typedef struct MockAllocator {
    IEcoMemoryAllocator1 allocator;
    IEcoMemoryAllocator1VTbl vtbl;
    uint32_t references, allocations, frees, live, invalid, calls, fail_at;
    void* blocks[TEST_SLOTS];
    uint32_t sizes[TEST_SLOTS];
} MockAllocator;
static uint32_t ECOCALLMETHOD mem_add(IEcoMemoryAllocator1* me) {
    if (me == 0) return 0u;
    return ++((MockAllocator*)me)->references;
}
static uint32_t ECOCALLMETHOD mem_release(IEcoMemoryAllocator1* me) {
    MockAllocator* memory;
    if (me == 0) return 0u;
    memory = (MockAllocator*)me;
    if (memory->references != 0u) --memory->references;
    return memory->references;
}
static int16_t ECOCALLMETHOD mem_qi(IEcoMemoryAllocator1* me,
    const UGUID* id, voidptr_t* out) {
    const unsigned char* a; const unsigned char* b; uint32_t i; int match;
    if (out == 0) return ERR_ECO_POINTER;
    *out = 0;
    if (me == 0 || id == 0) return ERR_ECO_POINTER;
    a = (const unsigned char*)id;
    b = (const unsigned char*)&IID_IEcoMemoryAllocator1;
    match = 1;
    for (i = 0u; i < 18u; ++i) if (a[i] != b[i]) match = 0;
    if (!match) {
        b = (const unsigned char*)&IID_IEcoUnknown; match = 1;
        for (i = 0u; i < 18u; ++i) if (a[i] != b[i]) match = 0;
    }
    if (!match) return ERR_ECO_NOINTERFACE;
    mem_add(me); *out = me; return ERR_ECO_SUCCESS;
}
static voidptr_t ECOCALLMETHOD mem_alloc(IEcoMemoryAllocator1* me, uint32_t size) {
    MockAllocator* memory; uint32_t i; void* block;
    if (me == 0) return 0;
    memory = (MockAllocator*)me; ++memory->calls;
    if (memory->fail_at != 0u && memory->calls == memory->fail_at) return 0;
    for (i = 0u; i < TEST_SLOTS; ++i) if (memory->blocks[i] == 0) break;
    if (i == TEST_SLOTS) { ++memory->invalid; return 0; }
    block = malloc(size == 0u ? 1u : size);
    if (block == 0) return 0;
    memory->blocks[i] = block; memory->sizes[i] = size;
    ++memory->allocations; ++memory->live;
    return block;
}
static void ECOCALLMETHOD mem_free(IEcoMemoryAllocator1* me, voidptr_t value) {
    MockAllocator* memory; uint32_t i;
    if (me == 0 || value == 0) return;
    memory = (MockAllocator*)me;
    for (i = 0u; i < TEST_SLOTS; ++i) if (memory->blocks[i] == value) break;
    if (i == TEST_SLOTS) { ++memory->invalid; return; }
    free(value); memory->blocks[i] = 0; memory->sizes[i] = 0u;
    ++memory->frees; --memory->live;
}
static voidptr_t ECOCALLMETHOD mem_copy(IEcoMemoryAllocator1* me,
    voidptr_t dst, voidptr_t src, uint32_t size) {
    unsigned char* target; unsigned char* source; uint32_t i;
    (void)me; target = (unsigned char*)dst; source = (unsigned char*)src;
    for (i = 0u; i < size; ++i) target[i] = source[i];
    return dst;
}
static uint32_t ECOCALLMETHOD mem_size(IEcoMemoryAllocator1* me, voidptr_t value) {
    MockAllocator* memory; uint32_t i;
    if (me == 0 || value == 0) return 0u;
    memory = (MockAllocator*)me;
    for (i = 0u; i < TEST_SLOTS; ++i)
        if (memory->blocks[i] == value) return memory->sizes[i];
    ++memory->invalid; return 0u;
}
static voidptr_t ECOCALLMETHOD mem_realloc(IEcoMemoryAllocator1* me,
    voidptr_t value, uint32_t size) {
    void* block; uint32_t old_size;
    if (value == 0) return mem_alloc(me, size);
    if (size == 0u) { mem_free(me, value); return 0; }
    old_size = mem_size(me, value); block = mem_alloc(me, size);
    if (block == 0) return 0;
    mem_copy(me, block, value, old_size < size ? old_size : size);
    mem_free(me, value); return block;
}
static voidptr_t ECOCALLMETHOD mem_fill(IEcoMemoryAllocator1* me,
    voidptr_t dst, char_t value, uint32_t size) {
    unsigned char* target; uint32_t i;
    (void)me; target = (unsigned char*)dst;
    for (i = 0u; i < size; ++i) target[i] = (unsigned char)value;
    return dst;
}
static int16_t ECOCALLMETHOD mem_compare(IEcoMemoryAllocator1* me,
    voidptr_t a, voidptr_t b, uint32_t size) {
    unsigned char* left; unsigned char* right; uint32_t i;
    (void)me; left = (unsigned char*)a; right = (unsigned char*)b;
    for (i = 0u; i < size; ++i)
        if (left[i] != right[i]) return left[i] < right[i] ? -1 : 1;
    return 0;
}
static void allocator_init(MockAllocator* memory) {
    uint32_t i;
    memory->vtbl.QueryInterface = mem_qi;
    memory->vtbl.AddRef = mem_add; memory->vtbl.Release = mem_release;
    memory->vtbl.Alloc = mem_alloc; memory->vtbl.Free = mem_free;
    memory->vtbl.Realloc = mem_realloc; memory->vtbl.Copy = mem_copy;
    memory->vtbl.Fill = mem_fill; memory->vtbl.Compare = mem_compare;
    memory->vtbl.RetrieveSize = mem_size;
    memory->allocator.pVTbl = &memory->vtbl;
    memory->references = 1u; memory->allocations = 0u; memory->frees = 0u;
    memory->live = 0u; memory->invalid = 0u; memory->calls = 0u; memory->fail_at = 0u;
    for (i = 0u; i < TEST_SLOTS; ++i) memory->blocks[i] = 0;
}

static double abs_double(double value) {
    return value < 0.0 ? -value : value;
}
static int close_to(double actual, double expected, double tolerance) {
    return abs_double(actual - expected) <= tolerance;
}
static int close_long_double(long double actual, long double expected,
    long double tolerance) {
    return actual - expected <= tolerance && expected - actual <= tolerance;
}

static uint32_t current_method;
static uint32_t tested_algorithms;
static uint32_t checks;
static uint32_t failures;
static void check_at(int condition, const char* file, int line) {
    ++checks;
    if (!condition) { ++failures; printf("FAIL %s:%d\n", file, line); }
}
#define check(c) check_at((c), __FILE__, __LINE__)
static void reference_row(const char* name, const double* values, uint32_t count) {
    uint32_t i;
    printf("REFERENCE {\"case\":\"%s\",\"method\":%u,\"coefficients\":[", name, current_method);
    for (i = 0u; i < count; ++i) printf("%s%.17g", i == 0u ? "" : ",", values[i]);
    printf("]}\n");
}
static void check_quadratic(IEcoPolynomialMath* component) {
    const double x[5] = {-2.0, -1.0, 0.0, 1.0, 2.0};
    const double y[5] = {11.0, 5.0, 3.0, 5.0, 11.0};
    double coefficients[3];
    int16_t status;
    status = component->pVTbl->polyfit(component, x, y, 5u, 2u, coefficients);
    check(status == ERR_ECO_SUCCESS);
    check(close_to(coefficients[0], 2.0, 1.0e-9));
    check(close_to(coefficients[1], 0.0, 1.0e-9));
    check(close_to(coefficients[2], 3.0, 1.0e-9));
    reference_row("quadratic", coefficients, 3u);
}
static void check_cubic(IEcoPolynomialMath* component) {
    const double x[6] = {-2.0, -1.0, 0.0, 1.0, 2.0, 3.0};
    const double y[6] = {-14.0, -0.5, 3.0, 2.5, 4.0, 13.5};
    double coefficients[4];
    int16_t status;
    status = component->pVTbl->polyfit(component, x, y, 6u, 3u, coefficients);
    check(status == ERR_ECO_SUCCESS);
    check(close_to(coefficients[0], 1.0, 1.0e-8));
    check(close_to(coefficients[1], -2.0, 1.0e-8));
    check(close_to(coefficients[2], 0.5, 1.0e-8));
    check(close_to(coefficients[3], 3.0, 1.0e-8));
    reference_row("cubic", coefficients, 4u);
}
static void check_constant_and_scale(IEcoPolynomialMath* component) {
    const double constant_x[4] = {-10.0, -2.0, 4.0, 9.0};
    const double constant_y[4] = {7.0, 7.0, 7.0, 7.0};
    const double scaled_x[5] = {1000000.0, 1000001.0, 1000002.0,
        1000003.0, 1000004.0};
    const double scaled_y[5] = {2000003.0, 2000005.0, 2000007.0,
        2000009.0, 2000011.0};
    double coefficients[4];
    int16_t status;
    status = component->pVTbl->polyfit(component, constant_x, constant_y,
        4u, 0u, coefficients);
    check(status == ERR_ECO_SUCCESS);
    check(close_to(coefficients[0], 7.0, 1.0e-9));
    reference_row("constant", coefficients, 1u);
    status = component->pVTbl->polyfit(component, scaled_x, scaled_y,
        5u, 1u, coefficients);
    check(status == ERR_ECO_SUCCESS);
    check(close_to(coefficients[0], 2.0, 1.0e-6));
    check(close_to(coefficients[1], 3.0, 1.0e-3));
    reference_row("large_offset", coefficients, 2u);
}
static void check_overloads(IEcoPolynomialMath* component) {
    const float xf[5] = {-2.0f, -1.0f, 0.0f, 1.0f, 2.0f};
    const float yf[5] = {11.0f, 5.0f, 3.0f, 5.0f, 11.0f};
    const int xi[5] = {-2, -1, 0, 1, 2};
    const int yi[5] = {11, 5, 3, 5, 11};
    const long xl[5] = {-2L, -1L, 0L, 1L, 2L};
    const long yl[5] = {11L, 5L, 3L, 5L, 11L};
    const long double xld[5] = {-2.0L, -1.0L, 0.0L, 1.0L, 2.0L};
    const long double yld[5] = {11.0L, 5.0L, 3.0L, 5.0L, 11.0L};
    float cf[3];
    double ci[3];
    long double cl[3];
    int16_t status;
    status = component->pVTbl->polyfitf(component, xf, yf, 5u, 2u, cf);
    check(status == ERR_ECO_SUCCESS && close_to(cf[0], 2.0, 1.0e-4) && close_to(cf[1], 0.0, 1.0e-4) && close_to(cf[2], 3.0, 1.0e-4));
    status = component->pVTbl->polyfiti(component, xi, yi, 5u, 2u, ci);
    check(status == ERR_ECO_SUCCESS && close_to(ci[0], 2.0, 1.0e-9) && close_to(ci[1], 0.0, 1.0e-9) && close_to(ci[2], 3.0, 1.0e-9));
    status = component->pVTbl->polyfit_long(component, xl, yl, 5u, 2u, cl);
    check(status == ERR_ECO_SUCCESS && close_long_double(cl[0], 2.0L, 1.0e-9L) && close_long_double(cl[1], 0.0L, 1.0e-9L) && close_long_double(cl[2], 3.0L, 1.0e-9L));
    status = component->pVTbl->polyfitl(component, xld, yld, 5u, 2u, cl);
    check(status == ERR_ECO_SUCCESS && close_long_double(cl[0], 2.0L, 1.0e-9L) && close_long_double(cl[1], 0.0L, 1.0e-9L) && close_long_double(cl[2], 3.0L, 1.0e-9L));
}
static void check_object_and_services(IEcoPolynomialMath* component) {
    const double x[5] = {-2.0, -1.0, 0.0, 1.0, 2.0};
    const double y[5] = {11.0, 5.0, 3.0, 5.0, 11.0};
    EcoVectorView vx;
    EcoVectorView vy;
    IEcoPolynomial* polynomial;
    IEcoPolynomial* queried;
    double coefficients[3];
    double value;
    uint32_t degree;
    int16_t status;
    vx.data = x; vx.length = 5u;
    vy.data = y; vy.length = 5u;
    status = component->pVTbl->FitVector(component, &vx, &vy, 2u, coefficients);
    check(status == ERR_ECO_SUCCESS && close_to(coefficients[2], 3.0, 1.0e-9));
    status = component->pVTbl->Add(component, 2.0, 3.0, &value);
    check(status == ERR_ECO_SUCCESS && close_to(value, 5.0, 1.0e-12));
    status = component->pVTbl->Subtract(component, 7.0, 4.0, &value);
    check(status == ERR_ECO_SUCCESS && close_to(value, 3.0, 1.0e-12));
    polynomial = 0;
    status = component->pVTbl->new_Polynomial(component, x, y, 5u, 2u,
        &polynomial);
    check(status == ERR_ECO_SUCCESS && polynomial != 0);
    if (polynomial == 0) return;
    status = polynomial->pVTbl->get_Degree(polynomial, &degree);
    check(status == ERR_ECO_SUCCESS && degree == 2u);
    status = polynomial->pVTbl->get_Coefficients(polynomial, 3u, coefficients);
    check(status == ERR_ECO_SUCCESS && close_to(coefficients[0], 2.0, 1.0e-9));
    status = polynomial->pVTbl->Evaluate(polynomial, 2.0, &value);
    check(status == ERR_ECO_SUCCESS && close_to(value, 11.0, 1.0e-9));
    queried = 0;
    status = polynomial->pVTbl->QueryInterface(polynomial, &IID_IEcoPolynomial,
        (voidptr_t*)&queried);
    check(status == ERR_ECO_SUCCESS && queried != 0);
    if (queried != 0) queried->pVTbl->Release(queried);
    if (polynomial != 0) polynomial->pVTbl->Release(polynomial);
}
static void check_errors(IEcoPolynomialMath* component) {
    const double x[5] = {-2.0, -1.0, 0.0, 1.0, 2.0};
    double y[5] = {11.0, 5.0, 3.0, 5.0, 11.0};
    double coefficients[3];
    UGUID unknown_id;
    voidptr_t unknown;
    int16_t status;
    volatile double zero;
    unknown_id.Preamble = 1u;
    unknown_id.Length = 16u;
    unknown_id.Data[0] = 0xAAu;
    unknown_id.Data[1] = 0x55u;
    unknown_id.Data[2] = 0u;
    unknown_id.Data[3] = 0u;
    unknown_id.Data[4] = 0u;
    unknown_id.Data[5] = 0u;
    unknown_id.Data[6] = 0u;
    unknown_id.Data[7] = 0u;
    unknown_id.Data[8] = 0u;
    unknown_id.Data[9] = 0u;
    unknown_id.Data[10] = 0u;
    unknown_id.Data[11] = 0u;
    unknown_id.Data[12] = 0u;
    unknown_id.Data[13] = 0u;
    unknown_id.Data[14] = 0u;
    unknown_id.Data[15] = 0u;
    unknown = 0;
    status = component->pVTbl->QueryInterface(component, &unknown_id, &unknown);
    check(status == ERR_ECO_NOINTERFACE && unknown == 0);
    status = component->pVTbl->polyfit(component, 0, y, 5u, 2u, coefficients);
    check(status == ERR_ECO_POINTER);
    status = component->pVTbl->polyfit(component, x, y, 2u, 2u, coefficients);
    check(status == ERR__POLYFIT_ARGUMENT);
    status = component->pVTbl->polyfit(component, x, y, 5u, 17u, coefficients);
    check(status == ERR__POLYFIT_ARGUMENT);
    status = component->pVTbl->polyfit(component, x, y, 5u, 2u, 0);
    check(status == ERR_ECO_POINTER);
    zero = 0.0;
    y[0] = y[0] / zero;
    status = component->pVTbl->polyfit(component, x, y, 5u, 2u, coefficients);
    check(status == ERR__POLYFIT_NONFINITE);
}
#define REPORT_ROW(name,values,count) reference_row(name,values,count)
#include "MathRegression.h"
#undef REPORT_ROW
static void check_component(MockAllocator* memory, IEcoPolynomialMath* component) {
    (void)memory;
    check(component != 0);
    if (component == 0) return;
    check_quadratic(component);
    check_cubic(component);
    check_constant_and_scale(component);
    check_overloads(component);
    check_object_and_services(component);
    check_errors(component);
    check_extended_math(component);
    tested_algorithms |= 1u << current_method;
    component->pVTbl->AddRef(component);
    check(component->pVTbl->Release(component) != 0u);
}

static void check_allocation_failures(void) {
    MockAllocator memory; IEcoPolynomialMath* component; uint32_t fail; int16_t status;
    int succeeded; const double x[3] = {-1.0, 0.0, 1.0};
    const double y[3] = {1.0, 0.0, 1.0}; double out[3]; IEcoPolynomial* polynomial;
    succeeded = 0;
    for (fail = 1u; fail <= 32u; ++fail) {
        allocator_init(&memory); component = 0; memory.fail_at = fail;
        status = PolyfitComponent_Create_00000045(0, &memory.allocator, 0u, 0, &component);
        check((status == ERR_ECO_SUCCESS) == (component != 0));
        if (component != 0) {
            component->pVTbl->Release(component); succeeded = 1;
        }
        check(memory.live == 0u && memory.allocations == memory.frees);
        check(memory.references == 1u && memory.invalid == 0u);
        if (succeeded) break;
    }
    check(succeeded);
    allocator_init(&memory); component = 0;
    status = PolyfitComponent_Create_00000045(0, &memory.allocator, 0u, 0, &component);
    check(status == ERR_ECO_SUCCESS && component != 0);
    if (component != 0) {
        for (fail = 1u; fail <= 3u; ++fail) {
            memory.calls = 0u; memory.fail_at = fail;
            status = component->pVTbl->polyfit(component, x, y, 3u, 2u, out);
            check(fail <= 2u ? status == ERR_ECO_OUTOFMEMORY : status == ERR_ECO_SUCCESS);
        }
        succeeded = 0;
        for (fail = 1u; fail <= 8u; ++fail) {
            memory.calls = 0u; memory.fail_at = fail; polynomial = 0;
            status = component->pVTbl->new_Polynomial(component, x, y, 3u, 2u, &polynomial);
            check((status == ERR_ECO_SUCCESS) == (polynomial != 0));
            if (polynomial != 0) { polynomial->pVTbl->Release(polynomial); succeeded = 1; }
            if (succeeded) break;
        }
        check(succeeded); memory.fail_at = 0u; component->pVTbl->Release(component);
    }
    check(memory.live == 0u && memory.allocations == memory.frees);
    check(memory.references == 1u && memory.invalid == 0u);
}
int main(void) {
    MockAllocator memory;
    IEcoPolynomialMath* normal;
    IEcoPolynomialMath* qr;
    int16_t status;
    check_allocation_failures();
    allocator_init(&memory);
    normal = 0;
    qr = 0;
    status = PolyfitComponent_Create_00000045(0, &memory.allocator, 0u, 0, &normal);
    check(status == ERR_ECO_SUCCESS);
    current_method = 0u; check_component(&memory, normal);
    status = PolyfitComponent_Create_00000045(0, &memory.allocator, 1u, 0, &qr);
    check(status == ERR_ECO_SUCCESS);
    current_method = 1u; check_component(&memory, qr);
    if (normal != 0) normal->pVTbl->Release(normal);
    if (qr != 0) qr->pVTbl->Release(qr);
    check(memory.allocations > 0u);
    check(memory.frees == memory.allocations);
    check(memory.live == 0u && memory.invalid == 0u);
    check(memory.references == 1u);
    check(tested_algorithms == 3u);
    printf("Проверок: %lu, ошибок: %lu\n", (unsigned long)checks,
        (unsigned long)failures);
    return failures == 0u ? 0 : 1;
}

