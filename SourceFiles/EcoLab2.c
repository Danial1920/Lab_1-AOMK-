/* Lab2 aggregates Lab1 through its nondelegating unknown and includes E.
 * Its Y facade keeps the external identity while forwarding calls to E. */
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
    if (component->included_e != 0)
        component->included_e->pVTbl->Release(component->included_e);
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
    if (guid_equal(riid, &IID_IEcoCalculatorY)) {
        ++component->references; *ppv = &component->calculator_y.iface;
        return ERR_ECO_SUCCESS;
    }
    if (component->lab1_inner != 0 &&
        (guid_equal(riid, &IID_IEcoLab1) || guid_equal(riid, &IID_IEcoPolynomialMath) ||
         guid_equal(riid, &IID_IEcoCalculatorX)))
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
static IEcoLab2VTbl lab2_vtbl = {lab2_qi, lab2_add_ref, lab2_release};
static int16_t ECOCALLMETHOD lab2_y_qi(IEcoCalculatorYPtr me,
    const UGUID* riid, voidptr_t* ppv) {
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    return lab2_qi(&((CEcoLab2YView*)me)->owner->lab2, riid, ppv);
}
static uint32_t ECOCALLMETHOD lab2_y_add_ref(IEcoCalculatorYPtr me) {
    if (me == 0) return 0u;
    return lab2_add_ref(&((CEcoLab2YView*)me)->owner->lab2);
}
static uint32_t ECOCALLMETHOD lab2_y_release(IEcoCalculatorYPtr me) {
    if (me == 0) return 0u;
    return lab2_release(&((CEcoLab2YView*)me)->owner->lab2);
}
static double ECOCALLMETHOD lab2_multiply(IEcoCalculatorYPtr me, double a, double b) {
    IEcoCalculatorY* child;
    if (me == 0) return 0.0;
    child = ((CEcoLab2YView*)me)->owner->included_e;
    return child->pVTbl->Multiplication(child, a, b);
}
static double ECOCALLMETHOD lab2_divide(IEcoCalculatorYPtr me, double a, double b) {
    IEcoCalculatorY* child;
    if (me == 0) return 0.0;
    child = ((CEcoLab2YView*)me)->owner->included_e;
    return child->pVTbl->Division(child, a, b);
}
static int16_t ECOCALLMETHOD lab2_try_divide(IEcoCalculatorYPtr me,
    double a, double b, double* result) {
    IEcoCalculatorY* child;
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0.0;
    if (me == 0) return ERR_ECO_POINTER;
    child = ((CEcoLab2YView*)me)->owner->included_e;
    return child->pVTbl->TryDivision(child, a, b, result);
}
static IEcoCalculatorYVTbl lab2_y_vtbl = {
    lab2_y_qi, lab2_y_add_ref, lab2_y_release,
    lab2_multiply, lab2_divide, lab2_try_divide
};
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
    component->calculator_y.iface.pVTbl = &lab2_y_vtbl;
    component->calculator_y.owner = component;
    component->references = 1u;
    component->memory = memory; memory->pVTbl->AddRef(memory);
    component->system = 0; component->lab1_inner = 0; component->included_e = 0;
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
        status = bus->pVTbl->QueryComponent(bus, &CID_EcoCalculatorE, 0,
            &IID_IEcoCalculatorY, (voidptr_t*)&component->included_e);
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
static char_t* ECOCALLMETHOD factory_version(IEcoComponentFactoryPtr_t me) { if (me == 0) return ""; return "1.0.0"; }
static char_t* ECOCALLMETHOD factory_manufacturer(IEcoComponentFactoryPtr_t me) { if (me == 0) return ""; return "Laboratory 2"; }
static IEcoComponentFactoryVTbl factory_vtbl = {factory_qi, factory_add_ref,
    factory_release, factory_alloc, factory_init, factory_name,
    factory_version, factory_manufacturer};
static CEcoLab2Factory lab2_factory = {{&factory_vtbl},1u};
IEcoComponentFactory* GetEcoLab2Factory(void) { return &lab2_factory.factory; }

int16_t EcoLab2_RegisterOnBus(IEcoInterfaceBus1Ptr_t bus) {
    int16_t status;
    if (bus == 0) return ERR_ECO_POINTER;
    status = PolyfitComponent_RegisterOnBus(bus);
    if (status != ERR_ECO_SUCCESS) return status;
    status = CalculatorComponents_RegisterOnBus(bus);
    if (status != ERR_ECO_SUCCESS) {
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMathQR);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMath);
        return status;
    }
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoLab2,
        (IEcoUnknownPtr_t)GetEcoLab2Factory());
    if (status != ERR_ECO_SUCCESS) {
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorE);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorD);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorC);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorB);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorA);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMathQR);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMath);
    }
    return status;
}
