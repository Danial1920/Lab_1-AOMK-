/* Eco.Lab2: external component demonstrating inclusion and aggregation. */
#include "CEcoLab2.h"
#include "polyfit_component.h"
#include "polyfit_bus.h"
#include "IdEcoLab2.h"
#include "IdEcoPolynomialMath.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "Err.h"

typedef struct CEcoLab2Factory {
    IEcoComponentFactory factory;
    uint32_t references;
} CEcoLab2Factory;

static int guid_equal(const UGUID* a, const UGUID* b) {
    const byte_t* left;
    const byte_t* right;
    uint32_t i;
    if (a == 0 || b == 0) return 0;
    left = (const byte_t*)a; right = (const byte_t*)b;
    for (i = 0u; i < 18u; ++i) if (left[i] != right[i]) return 0;
    return 1;
}

static void lab2_destroy(CEcoLab2Component* component) {
    CCalculatorComponent* calculator;
    IEcoMemoryAllocator1* memory;
    if (component == 0) return;
    memory = component->memory;
    /* calculator_x and calculator_y are borrowed views of aggregated Lab1. */
    if (component->calculator_c != 0) component->calculator_c->pVTbl->Release(component->calculator_c);
    if (component->calculator_d != 0) component->calculator_d->pVTbl->Release(component->calculator_d);
    if (component->calculator_b != 0) {
        calculator = Calculator_FromX(component->calculator_b);
        Calculator_Destroy(calculator);
    }
    if (component->calculator_e != 0) {
        calculator = Calculator_FromY(component->calculator_e);
        Calculator_Destroy(calculator);
    }
    if (component->lab1 != 0) {
        IEcoPolynomialMath* math;
        math = 0;
        component->lab1->pVTbl->QueryInterface(component->lab1,
            &IID_IEcoPolynomialMath, (voidptr_t*)&math);
        if (math != 0) {
            PolyfitComponent_Destroy_00000045(math);
        }
    }
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
    if (guid_equal(riid, &IID_IEcoLab1) && component->lab1 != 0) {
        component->lab1->pVTbl->AddRef(component->lab1); *ppv = component->lab1;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoCalculatorX) && component->calculator_x != 0) {
        component->calculator_x->pVTbl->AddRef(component->calculator_x);
        *ppv = component->calculator_x; return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoCalculatorY) && component->calculator_y != 0) {
        component->calculator_y->pVTbl->AddRef(component->calculator_y);
        *ppv = component->calculator_y; return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD lab2_add_ref(IEcoLab2Ptr me) {
    CEcoLab2Component* component;
    if (me == 0) return 0u;
    component = (CEcoLab2Component*)me; ++component->references;
    return component->references;
}
static uint32_t ECOCALLMETHOD lab2_release(IEcoLab2Ptr me) {
    CEcoLab2Component* component;
    uint32_t references;
    if (me == 0) return 0u;
    component = (CEcoLab2Component*)me;
    if (component->references == 0u) return 0u;
    --component->references;
    references = component->references;
    if (references == 0u) lab2_destroy(component);
    return references;
}
static IEcoLab2VTbl lab2_vtbl = {lab2_qi, lab2_add_ref, lab2_release};

static int16_t lab2_query(IEcoInterfaceBus1* bus, const UGUID* cid,
    IEcoUnknown* outer, const UGUID* iid, voidptr_t* value) {
    return bus->pVTbl->QueryComponent(bus, cid, outer, iid, value);
}

static int16_t EcoLab2_Create(IEcoUnknown* system, IEcoMemoryAllocator1* memory,
    CEcoLab2Component** result) {
    CEcoLab2Component* component;
    IEcoInterfaceBus1* bus;
    int16_t status;
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0;
    if (system == 0 || memory == 0) return ERR_ECO_POINTER;
    component = (CEcoLab2Component*)memory->pVTbl->Alloc(memory,
        (uint32_t)sizeof(CEcoLab2Component));
    if (component == 0) return ERR_ECO_OUTOFMEMORY;
    component->lab2.pVTbl = &lab2_vtbl;
    component->references = 1u;
    component->memory = memory; memory->pVTbl->AddRef(memory);
    component->system = 0; component->lab1 = 0;
    component->calculator_x = 0; component->calculator_y = 0;
    component->calculator_c = 0; component->calculator_d = 0;
    component->calculator_b = 0; component->calculator_e = 0;
    status = system->pVTbl->QueryInterface(system, &GID_IEcoSystem,
        (voidptr_t*)&component->system);
    if (status != ERR_ECO_SUCCESS || component->system == 0) {
        memory->pVTbl->Free(memory, component); memory->pVTbl->Release(memory);
        return ERR_ECO_NOSYSTEM;
    }
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&bus);
    if (status != ERR_ECO_SUCCESS || bus == 0) { lab2_destroy(component); return status; }

    /* Eco.Lab1 is aggregated by Lab2; A, C and D are included normally. */
    status = lab2_query(bus, &CID_EcoPolynomialMath,
        (IEcoUnknown*)&component->lab2, &IID_IEcoLab1,
        (voidptr_t*)&component->lab1);
    if (status == ERR_ECO_SUCCESS)
        component->lab1->pVTbl->Release(component->lab1);
    if (status == ERR_ECO_SUCCESS)
        status = component->lab1->pVTbl->QueryInterface(component->lab1,
            &IID_IEcoCalculatorX, (voidptr_t*)&component->calculator_x);
    if (status == ERR_ECO_SUCCESS)
        component->calculator_x->pVTbl->Release(component->calculator_x);
    if (status == ERR_ECO_SUCCESS)
        status = component->lab1->pVTbl->QueryInterface(component->lab1,
            &IID_IEcoCalculatorY, (voidptr_t*)&component->calculator_y);
    if (status == ERR_ECO_SUCCESS)
        component->calculator_y->pVTbl->Release(component->calculator_y);
    if (status == ERR_ECO_SUCCESS)
        status = lab2_query(bus, &CID_EcoCalculatorA, 0,
            &IID_IEcoCalculatorX, (voidptr_t*)&component->calculator_c);
    if (status == ERR_ECO_SUCCESS)
        status = lab2_query(bus, &CID_EcoCalculatorD, 0,
            &IID_IEcoCalculatorY, (voidptr_t*)&component->calculator_d);
    if (status == ERR_ECO_SUCCESS)
        status = lab2_query(bus, &CID_EcoCalculatorB,
            (IEcoUnknown*)&component->lab2, &IID_IEcoCalculatorX,
            (voidptr_t*)&component->calculator_b);
    if (status == ERR_ECO_SUCCESS)
        component->calculator_b->pVTbl->Release(component->calculator_b);
    if (status == ERR_ECO_SUCCESS)
        status = lab2_query(bus, &CID_EcoCalculatorE,
            (IEcoUnknown*)&component->lab2, &IID_IEcoCalculatorY,
            (voidptr_t*)&component->calculator_e);
    if (status == ERR_ECO_SUCCESS)
        component->calculator_e->pVTbl->Release(component->calculator_e);
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
static char_t* ECOCALLMETHOD factory_name(IEcoComponentFactoryPtr_t me) { (void)me; return "EcoLab2"; }
static char_t* ECOCALLMETHOD factory_version(IEcoComponentFactoryPtr_t me) { (void)me; return "1.0.0"; }
static char_t* ECOCALLMETHOD factory_manufacturer(IEcoComponentFactoryPtr_t me) { (void)me; return "Laboratory 2"; }
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
