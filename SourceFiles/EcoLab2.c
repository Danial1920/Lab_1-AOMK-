/* Lab2 aggregates Lab1 (which aggregates B) and includes A and D for Horner. */
#include "CEcoLab2.h"
#include "polyfit_component.h"
#include "polyfit_bus.h"
#include "IdEcoLab2.h"
#include "IdEcoPolynomialMath.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "Err.h"

typedef struct CEcoLab2Factory {
    IEcoComponentFactory factory;
    uint32_t references;
} CEcoLab2Factory;
static int guid_equal(const UGUID* a, const UGUID* b) {
    const byte_t* left; const byte_t* right; uint32_t i;
    if (a == 0 || b == 0) return 0;
    left = (const byte_t*)a; right = (const byte_t*)b;
    for (i = 0u; i < 18u; ++i) if (left[i] != right[i]) return 0;
    return 1;
}
static void lab2_destroy(CEcoLab2Component* component) {
    IEcoMemoryAllocator1* memory;
    memory = component->memory;
    /* Private Release does not re-enter the dying controlling unknown. */
    if (component->lab1_inner != 0)
        component->lab1_inner->pVTbl->Release(component->lab1_inner);
    if (component->included_a != 0)
        component->included_a->pVTbl->Release(component->included_a);
    if (component->included_d != 0)
        component->included_d->pVTbl->Release(component->included_d);
    if (component->system != 0) component->system->pVTbl->Release(component->system);
    memory->pVTbl->Free(memory, component);
    memory->pVTbl->Release(memory);
}
static int16_t ECOCALLMETHOD lab2_qi(IEcoLab2Ptr me, const UGUID* riid,
    voidptr_t* ppv) {
    CEcoLab2Component* component;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    component = (CEcoLab2Component*)me;
    if (guid_equal(riid, &IID_IEcoLab2) || guid_equal(riid, &IID_IEcoUnknown)) {
        ++component->references; *ppv = me; return ERR_ECO_SUCCESS;
    }
    if (component->lab1_inner != 0 &&
        (guid_equal(riid, &IID_IEcoLab1) || guid_equal(riid, &IID_IEcoPolynomialMath) ||
         guid_equal(riid, &IID_IEcoCalculatorX) || guid_equal(riid, &IID_IEcoCalculatorY) ||
         guid_equal(riid, &IID_IEcoCalculatorSafe)))
        return component->lab1_inner->pVTbl->QueryInterface(component->lab1_inner, riid, ppv);
    return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD lab2_add_ref(IEcoLab2Ptr me) {
    CEcoLab2Component* component;
    if (me == 0) return 0u;
    component = (CEcoLab2Component*)me;
    return ++component->references;
}
static uint32_t ECOCALLMETHOD lab2_release(IEcoLab2Ptr me) {
    CEcoLab2Component* component; uint32_t references;
    if (me == 0) return 0u;
    component = (CEcoLab2Component*)me;
    if (component->references == 0u) return 0u;
    references = --component->references;
    if (references == 0u) lab2_destroy(component);
    return references;
}
static int16_t ECOCALLMETHOD lab2_horner(IEcoLab2Ptr me, const int16_t* coefficients,
    uint32_t count, int16_t x, int32_t* result) {
    CEcoLab2Component* component; uint32_t i; int32_t value, product;
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0;
    if (me == 0 || coefficients == 0) return ERR_ECO_POINTER;
    if (count == 0u || count > 0xFFFFFFFFu / sizeof(int16_t)) return ERR_ECO_INVALIDARG;
    component = (CEcoLab2Component*)me;
    value = coefficients[0];
    for (i = 1u; i < count; ++i) {
        if (value < -32768L || value > 32767L) return ERR_ECO_CALCULATOR_OVERFLOW;
        product = component->included_d->pVTbl->Multiplication(component->included_d, (int16_t)value, x);
        if (product < -32768L || product > 32767L) return ERR_ECO_CALCULATOR_OVERFLOW;
        value = component->included_a->pVTbl->Addition(component->included_a, (int16_t)product, coefficients[i]);
    }
    *result = value;
    return ERR_ECO_SUCCESS;
}
static IEcoLab2VTbl lab2_vtbl = {lab2_qi, lab2_add_ref, lab2_release, lab2_horner};
static int16_t EcoLab2_Create(IEcoUnknown* system, IEcoMemoryAllocator1* memory,
    CEcoLab2Component** result) {
    CEcoLab2Component* component; IEcoInterfaceBus1* bus; int16_t status;
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0;
    if (system == 0 || memory == 0) return ERR_ECO_POINTER;
    component = (CEcoLab2Component*)memory->pVTbl->Alloc(memory,
        (uint32_t)sizeof(CEcoLab2Component));
    if (component == 0) return ERR_ECO_OUTOFMEMORY;
    component->lab2.pVTbl = &lab2_vtbl;
    component->references = 1u;
    component->memory = memory; memory->pVTbl->AddRef(memory);
    component->system = 0; component->lab1_inner = 0; component->included_a = 0; component->included_d = 0;
    bus = 0;
    status = system->pVTbl->QueryInterface(system, &GID_IEcoSystem,
        (voidptr_t*)&component->system);
    if (status != ERR_ECO_SUCCESS || component->system == 0) {
        lab2_destroy(component); return ERR_ECO_NOSYSTEM;
    }
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&bus);
    if (status != ERR_ECO_SUCCESS || bus == 0) {
        lab2_destroy(component); return ERR_ECO_GET_MEMORY_ALLOCATOR;
    }
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoPolynomialMath,
        (IEcoUnknown*)&component->lab2, &IID_IEcoUnknown,
        (voidptr_t*)&component->lab1_inner);
    if (status == ERR_ECO_SUCCESS)
        status = bus->pVTbl->QueryComponent(bus, &CID_EcoCalculatorA, 0,
            &IID_IEcoCalculatorX, (voidptr_t*)&component->included_a);
    if (status == ERR_ECO_SUCCESS)
        status = bus->pVTbl->QueryComponent(bus, &CID_EcoCalculatorD, 0,
            &IID_IEcoCalculatorY, (voidptr_t*)&component->included_d);
    bus->pVTbl->Release(bus);
    if (status != ERR_ECO_SUCCESS) { lab2_destroy(component); return status; }
    *result = component;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD factory_qi(IEcoComponentFactoryPtr_t me,
    const UGUID* riid, voidptr_t* ppv) {
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    if (guid_equal(riid, &IID_IEcoComponentFactory) || guid_equal(riid, &IID_IEcoUnknown)) {
        me->pVTbl->AddRef(me); *ppv = me; return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD factory_add_ref(IEcoComponentFactoryPtr_t me) {
    CEcoLab2Factory* factory;
    if (me == 0) return 0u; factory = (CEcoLab2Factory*)me; ++factory->references;
    return factory->references;
}
static uint32_t ECOCALLMETHOD factory_release(IEcoComponentFactoryPtr_t me) {
    CEcoLab2Factory* factory;
    if (me == 0) return 0u; factory = (CEcoLab2Factory*)me;
    if (factory->references != 0u) --factory->references; return factory->references;
}
static int16_t ECOCALLMETHOD factory_alloc(IEcoComponentFactoryPtr_t me,
    IEcoUnknown* system, IEcoUnknown* outer, const UGUID* riid, voidptr_t* ppv) {
    CEcoLab2Component* component; IEcoInterfaceBus1* bus;
    IEcoMemoryAllocator1* memory; int16_t status;
    if (ppv == 0) return ERR_ECO_POINTER; *ppv = 0;
    if (me == 0 || system == 0 || riid == 0) return ERR_ECO_POINTER;
    if (outer != 0) return ERR_ECO_NOAGGREGATION;
    bus = 0; memory = 0;
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&bus);
    if (status != ERR_ECO_SUCCESS || bus == 0) return ERR_ECO_GET_MEMORY_ALLOCATOR;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoMemoryManager1, 0,
        &IID_IEcoMemoryAllocator1, (voidptr_t*)&memory);
    bus->pVTbl->Release(bus);
    if (status != ERR_ECO_SUCCESS || memory == 0) return ERR_ECO_GET_MEMORY_ALLOCATOR;
    status = EcoLab2_Create(system, memory, &component);
    memory->pVTbl->Release(memory);
    if (status != ERR_ECO_SUCCESS) return status;
    status = component->lab2.pVTbl->QueryInterface(&component->lab2, riid, ppv);
    component->lab2.pVTbl->Release(&component->lab2);
    return status;
}
static int16_t ECOCALLMETHOD factory_init(IEcoComponentFactoryPtr_t me,
    IEcoUnknown* system, voidptr_t data) {
    if (me == 0) return ERR_ECO_POINTER; (void)system; (void)data; return ERR_ECO_SUCCESS;
}
static char_t* ECOCALLMETHOD factory_name(IEcoComponentFactoryPtr_t me) { if (me == 0) return ""; return "EcoLab2"; }
static char_t* ECOCALLMETHOD factory_version(IEcoComponentFactoryPtr_t me) { if (me == 0) return ""; return "2.0.0"; }
static char_t* ECOCALLMETHOD factory_manufacturer(IEcoComponentFactoryPtr_t me) { if (me == 0) return ""; return "Laboratory 2"; }
static IEcoComponentFactoryVTbl factory_vtbl = {factory_qi, factory_add_ref,
    factory_release, factory_alloc, factory_init, factory_name,
    factory_version, factory_manufacturer};
static CEcoLab2Factory lab2_factory = {{&factory_vtbl},1u};
IEcoComponentFactory* GetEcoLab2Factory(void) { return &lab2_factory.factory; }

#if defined(ECO_DLL) && defined(ECO_LAB2_ONLY)
#ifdef GetIEcoComponentFactoryPtr
#undef GetIEcoComponentFactoryPtr
#endif
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr(void) {
    return GetEcoLab2Factory();
}
#endif
