/* A-E calculator components for ACOM Lab 2. */
#include "CEcoLab2.h"
#include "IdEcoLab2.h"
#include "Err.h"
#include "IEcoInterfaceBus1.h"
#include "IdEcoMemoryManager1.h"

#define CALCULATOR_A 0u
#define CALCULATOR_B 1u
#define CALCULATOR_C 2u
#define CALCULATOR_D 3u
#define CALCULATOR_E 4u

typedef struct CCalculatorFactory {
    IEcoComponentFactory factory;
    uint32_t references;
    uint32_t kind;
    char_t* name;
} CCalculatorFactory;

static int guid_equal(const UGUID* a, const UGUID* b) {
    const byte_t* left;
    const byte_t* right;
    uint32_t i;
    if (a == 0 || b == 0) return 0;
    left = (const byte_t*)a;
    right = (const byte_t*)b;
    for (i = 0u; i < 18u; ++i) if (left[i] != right[i]) return 0;
    return 1;
}

static uint32_t calculator_add_ref(CCalculatorComponent* component) {
    if (component == 0) return 0u;
    if (component->outer != 0) return component->outer->pVTbl->AddRef(component->outer);
    ++component->references;
    return component->references;
}

void Calculator_Destroy(CCalculatorComponent* component) {
    IEcoMemoryAllocator1* memory;
    if (component == 0) return;
    memory = component->memory;
    memory->pVTbl->Free(memory, component);
    memory->pVTbl->Release(memory);
}
CCalculatorComponent* Calculator_FromX(IEcoCalculatorX* value) {
    if (value == 0) return 0;
    return ((CCalculatorXView*)value)->owner;
}
CCalculatorComponent* Calculator_FromY(IEcoCalculatorY* value) {
    if (value == 0) return 0;
    return ((CCalculatorYView*)value)->owner;
}

static uint32_t calculator_release(CCalculatorComponent* component) {
    uint32_t references;
    if (component == 0) return 0u;
    if (component->outer != 0) return component->outer->pVTbl->Release(component->outer);
    if (component->references == 0u) return 0u;
    --component->references;
    references = component->references;
    if (references == 0u) Calculator_Destroy(component);
    return references;
}

static int16_t ECOCALLMETHOD calculator_x_qi(IEcoCalculatorXPtr me,
    const UGUID* riid, voidptr_t* ppv) {
    CCalculatorComponent* component;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    component = ((CCalculatorXView*)me)->owner;
    if (guid_equal(riid, &IID_IEcoCalculatorX) ||
        (component->kind == CALCULATOR_C && guid_equal(riid, &IID_IEcoCalculatorY))) {
        calculator_add_ref(component);
        *ppv = guid_equal(riid, &IID_IEcoCalculatorX) ?
            (voidptr_t)&component->x.iface : (voidptr_t)&component->y.iface;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoUnknown)) {
        if (component->outer != 0)
            return component->outer->pVTbl->QueryInterface(component->outer, riid, ppv);
        calculator_add_ref(component);
        *ppv = (voidptr_t)&component->x.iface;
        return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}

static int16_t ECOCALLMETHOD calculator_y_qi(IEcoCalculatorYPtr me,
    const UGUID* riid, voidptr_t* ppv) {
    CCalculatorComponent* component;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    component = ((CCalculatorYView*)me)->owner;
    if (guid_equal(riid, &IID_IEcoCalculatorY) ||
        (component->kind == CALCULATOR_C && guid_equal(riid, &IID_IEcoCalculatorX))) {
        calculator_add_ref(component);
        *ppv = guid_equal(riid, &IID_IEcoCalculatorY) ?
            (voidptr_t)&component->y.iface : (voidptr_t)&component->x.iface;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoUnknown)) {
        if (component->outer != 0)
            return component->outer->pVTbl->QueryInterface(component->outer, riid, ppv);
        calculator_add_ref(component);
        *ppv = (voidptr_t)&component->y.iface;
        return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}

static uint32_t ECOCALLMETHOD calculator_x_add_ref(IEcoCalculatorXPtr me) {
    if (me == 0) return 0u;
    return calculator_add_ref(((CCalculatorXView*)me)->owner);
}
static uint32_t ECOCALLMETHOD calculator_x_release(IEcoCalculatorXPtr me) {
    if (me == 0) return 0u;
    return calculator_release(((CCalculatorXView*)me)->owner);
}
static uint32_t ECOCALLMETHOD calculator_y_add_ref(IEcoCalculatorYPtr me) {
    if (me == 0) return 0u;
    return calculator_add_ref(((CCalculatorYView*)me)->owner);
}
static uint32_t ECOCALLMETHOD calculator_y_release(IEcoCalculatorYPtr me) {
    if (me == 0) return 0u;
    return calculator_release(((CCalculatorYView*)me)->owner);
}

static double ECOCALLMETHOD calculator_add(IEcoCalculatorXPtr me, double a, double b) {
    if (me == 0) return 0.0;
    return a + b;
}
static double ECOCALLMETHOD calculator_subtract(IEcoCalculatorXPtr me, double a, double b) {
    if (me == 0) return 0.0;
    return a - b;
}
static double ECOCALLMETHOD calculator_multiply(IEcoCalculatorYPtr me, double a, double b) {
    if (me == 0) return 0.0;
    return a * b;
}
static double ECOCALLMETHOD calculator_divide(IEcoCalculatorYPtr me, double a, double b) {
    if (me == 0 || b == 0.0) return 0.0;
    return a / b;
}
static int16_t ECOCALLMETHOD calculator_try_divide(IEcoCalculatorYPtr me,
    double a, double b, double* result) {
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0.0;
    if (me == 0) return ERR_ECO_POINTER;
    if (b == 0.0) return ERR__POLYFIT_ARGUMENT;
    *result = a / b;
    return ERR_ECO_SUCCESS;
}

static IEcoCalculatorXVTbl calculator_x_vtbl = {
    calculator_x_qi, calculator_x_add_ref, calculator_x_release,
    calculator_add, calculator_subtract
};
static IEcoCalculatorYVTbl calculator_y_vtbl = {
    calculator_y_qi, calculator_y_add_ref, calculator_y_release,
    calculator_multiply, calculator_divide, calculator_try_divide
};

int16_t Calculator_Create(IEcoMemoryAllocator1* memory, uint32_t kind,
    IEcoUnknown* outer, CCalculatorComponent** result) {
    CCalculatorComponent* component;
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0;
    if (memory == 0 || kind > CALCULATOR_E) return ERR__POLYFIT_ARGUMENT;
    component = (CCalculatorComponent*)memory->pVTbl->Alloc(memory,
        (uint32_t)sizeof(CCalculatorComponent));
    if (component == 0) return ERR_ECO_OUTOFMEMORY;
    component->x.iface.pVTbl = &calculator_x_vtbl;
    component->y.iface.pVTbl = &calculator_y_vtbl;
    component->x.owner = component;
    component->y.owner = component;
    component->references = 1u;
    component->kind = kind;
    component->memory = memory;
    component->outer = outer;
    memory->pVTbl->AddRef(memory);
    *result = component;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD factory_qi(IEcoComponentFactoryPtr_t me,
    const UGUID* riid, voidptr_t* ppv) {
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    if (guid_equal(riid, &IID_IEcoComponentFactory) ||
        guid_equal(riid, &IID_IEcoUnknown)) {
        me->pVTbl->AddRef(me); *ppv = me; return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD factory_add_ref(IEcoComponentFactoryPtr_t me) {
    CCalculatorFactory* factory;
    if (me == 0) return 0u;
    factory = (CCalculatorFactory*)me; ++factory->references; return factory->references;
}
static uint32_t ECOCALLMETHOD factory_release(IEcoComponentFactoryPtr_t me) {
    CCalculatorFactory* factory;
    if (me == 0) return 0u;
    factory = (CCalculatorFactory*)me;
    if (factory->references != 0u) --factory->references;
    return factory->references;
}
static int16_t ECOCALLMETHOD factory_alloc(IEcoComponentFactoryPtr_t me,
    IEcoUnknownPtr_t system, IEcoUnknownPtr_t outer, const UGUID* riid,
    voidptr_t* ppv) {
    CCalculatorFactory* factory;
    IEcoInterfaceBus1* bus;
    IEcoMemoryAllocator1* memory;
    CCalculatorComponent* component;
    int16_t status;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || system == 0 || riid == 0) return ERR_ECO_POINTER;
    factory = (CCalculatorFactory*)me; bus = 0; memory = 0; component = 0;
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&bus);
    if (status != ERR_ECO_SUCCESS || bus == 0) return ERR_ECO_GET_MEMORY_ALLOCATOR;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoMemoryManager1, 0,
        &IID_IEcoMemoryAllocator1, (voidptr_t*)&memory);
    bus->pVTbl->Release(bus);
    if (status != ERR_ECO_SUCCESS || memory == 0) return ERR_ECO_GET_MEMORY_ALLOCATOR;
    status = Calculator_Create(memory, factory->kind, outer, &component);
    memory->pVTbl->Release(memory);
    if (status != ERR_ECO_SUCCESS) return status;
    if (factory->kind == CALCULATOR_D || factory->kind == CALCULATOR_E)
        status = component->y.iface.pVTbl->QueryInterface(&component->y.iface, riid, ppv);
    else
        status = component->x.iface.pVTbl->QueryInterface(&component->x.iface, riid, ppv);
    if (component->outer == 0) calculator_release(component);
    return status;
}
static int16_t ECOCALLMETHOD factory_init(IEcoComponentFactoryPtr_t me,
    IEcoUnknownPtr_t system, voidptr_t data) {
    if (me == 0) return ERR_ECO_POINTER;
    (void)system; (void)data; return ERR_ECO_SUCCESS;
}
static char_t* ECOCALLMETHOD factory_name(IEcoComponentFactoryPtr_t me) {
    if (me == 0) return ""; return ((CCalculatorFactory*)me)->name;
}
static char_t* ECOCALLMETHOD factory_version(IEcoComponentFactoryPtr_t me) {
    (void)me; return "1.0.0";
}
static char_t* ECOCALLMETHOD factory_manufacturer(IEcoComponentFactoryPtr_t me) {
    (void)me; return "Laboratory 2";
}
static IEcoComponentFactoryVTbl factory_vtbl = {
    factory_qi, factory_add_ref, factory_release, factory_alloc,
    factory_init, factory_name, factory_version, factory_manufacturer
};
static CCalculatorFactory factory_a = {{&factory_vtbl},1u,CALCULATOR_A,"CalculatorA"};
static CCalculatorFactory factory_b = {{&factory_vtbl},1u,CALCULATOR_B,"CalculatorB"};
static CCalculatorFactory factory_c = {{&factory_vtbl},1u,CALCULATOR_C,"CalculatorC"};
static CCalculatorFactory factory_d = {{&factory_vtbl},1u,CALCULATOR_D,"CalculatorD"};
static CCalculatorFactory factory_e = {{&factory_vtbl},1u,CALCULATOR_E,"CalculatorE"};
IEcoComponentFactory* GetCalculatorAFactory(void) { return &factory_a.factory; }
IEcoComponentFactory* GetCalculatorBFactory(void) { return &factory_b.factory; }
IEcoComponentFactory* GetCalculatorCFactory(void) { return &factory_c.factory; }
IEcoComponentFactory* GetCalculatorDFactory(void) { return &factory_d.factory; }
IEcoComponentFactory* GetCalculatorEFactory(void) { return &factory_e.factory; }

int16_t CalculatorComponents_RegisterOnBus(IEcoInterfaceBus1Ptr_t bus) {
    int16_t status;
    if (bus == 0) return ERR_ECO_POINTER;
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorA,
        (IEcoUnknownPtr_t)GetCalculatorAFactory()); if (status != ERR_ECO_SUCCESS) return status;
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorB,
        (IEcoUnknownPtr_t)GetCalculatorBFactory()); if (status != ERR_ECO_SUCCESS) goto rollback_a;
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorC,
        (IEcoUnknownPtr_t)GetCalculatorCFactory()); if (status != ERR_ECO_SUCCESS) goto rollback_b;
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorD,
        (IEcoUnknownPtr_t)GetCalculatorDFactory()); if (status != ERR_ECO_SUCCESS) goto rollback_c;
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoCalculatorE,
        (IEcoUnknownPtr_t)GetCalculatorEFactory());
    if (status == ERR_ECO_SUCCESS) return status;
    bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorD);
rollback_c:
    bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorC);
rollback_b:
    bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorB);
rollback_a:
    bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorA);
    return status;
}
