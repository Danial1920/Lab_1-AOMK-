/* A-E calculator components: inclusion uses owned public references;
 * aggregation owns a separate nondelegating unknown. */
#include "CEcoLab2.h"
#include "IdEcoLab2.h"
#include "Err.h"
#include "IdEcoMemoryManager1.h"

static int guid_equal(const UGUID* a, const UGUID* b) {
    const byte_t* left; const byte_t* right; uint32_t i;
    if (a == 0 || b == 0) return 0;
    left = (const byte_t*)a; right = (const byte_t*)b;
    for (i = 0u; i < 18u; ++i) if (left[i] != right[i]) return 0;
    return 1;
}
static int16_t ECOCALLMETHOD calculator_inner_qi(IEcoUnknown*, const UGUID*, voidptr_t*);
static uint32_t ECOCALLMETHOD calculator_inner_add_ref(IEcoUnknown*);
static uint32_t ECOCALLMETHOD calculator_inner_release(IEcoUnknown*);

static IEcoUnknown* calculator_controller(CCalculatorComponent* component) {
    return component->outer != 0 ? component->outer : &component->inner_unknown.iface;
}
static uint32_t calculator_add_ref(CCalculatorComponent* component) {
    IEcoUnknown* controller;
    if (component == 0) return 0u;
    controller = calculator_controller(component);
    return controller->pVTbl->AddRef(controller);
}
static uint32_t calculator_release(CCalculatorComponent* component) {
    IEcoUnknown* controller;
    if (component == 0) return 0u;
    controller = calculator_controller(component);
    return controller->pVTbl->Release(controller);
}
static void calculator_destroy(CCalculatorComponent* component) {
    IEcoMemoryAllocator1* memory;
    memory = component->memory;
    if (component->included_x != 0)
        component->included_x->pVTbl->Release(component->included_x);
    if (component->included_y != 0)
        component->included_y->pVTbl->Release(component->included_y);
    if (component->included_subtractor != 0)
        component->included_subtractor->pVTbl->Release(component->included_subtractor);
    if (component->aggregated_inner != 0)
        component->aggregated_inner->pVTbl->Release(component->aggregated_inner);
    memory->pVTbl->Free(memory, component);
    memory->pVTbl->Release(memory);
}
static uint32_t ECOCALLMETHOD calculator_inner_add_ref(IEcoUnknown* me) {
    CCalculatorComponent* component;
    if (me == 0) return 0u;
    component = ((CCalculatorUnknownView*)me)->owner;
    ++component->references;
    return component->references;
}
static uint32_t ECOCALLMETHOD calculator_inner_release(IEcoUnknown* me) {
    CCalculatorComponent* component; uint32_t references;
    if (me == 0) return 0u;
    component = ((CCalculatorUnknownView*)me)->owner;
    if (component->references == 0u) return 0u;
    references = --component->references;
    if (references == 0u) calculator_destroy(component);
    return references;
}
static int16_t ECOCALLMETHOD calculator_inner_qi(IEcoUnknown* me,
    const UGUID* riid, voidptr_t* ppv) {
    CCalculatorComponent* component;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    component = ((CCalculatorUnknownView*)me)->owner;
    if (guid_equal(riid, &IID_IEcoUnknown)) {
        calculator_inner_add_ref(me); *ppv = me; return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoCalculatorX)) {
        if (component->kind == CALCULATOR_E) {
            if (component->aggregated_inner == 0) return ERR_ECO_NOINTERFACE;
            return component->aggregated_inner->pVTbl->QueryInterface(
                component->aggregated_inner, riid, ppv);
        }
        if (component->kind > CALCULATOR_D) return ERR_ECO_NOINTERFACE;
        calculator_add_ref(component); *ppv = &component->x.iface;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoCalculatorY) && component->kind >= CALCULATOR_C) {
        calculator_add_ref(component); *ppv = &component->y.iface;
        return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}
static IEcoUnknownVTbl calculator_inner_vtbl = {
    calculator_inner_qi, calculator_inner_add_ref, calculator_inner_release
};
static int16_t calculator_public_qi(CCalculatorComponent* component,
    const UGUID* riid, voidptr_t* ppv) {
    IEcoUnknown* controller;
    controller = calculator_controller(component);
    return controller->pVTbl->QueryInterface(controller, riid, ppv);
}
static int16_t ECOCALLMETHOD calculator_x_qi(IEcoCalculatorXPtr me,
    const UGUID* riid, voidptr_t* ppv) {
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    return calculator_public_qi(((CCalculatorXView*)me)->owner, riid, ppv);
}
static int16_t ECOCALLMETHOD calculator_y_qi(IEcoCalculatorYPtr me,
    const UGUID* riid, voidptr_t* ppv) {
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    return calculator_public_qi(((CCalculatorYView*)me)->owner, riid, ppv);
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
    CCalculatorComponent* component;
    if (me == 0) return 0.0;
    component = ((CCalculatorXView*)me)->owner;
    if (component->included_x != 0)
        return component->included_x->pVTbl->Addition(component->included_x, a, b);
    return a + b;
}
static double ECOCALLMETHOD calculator_subtract(IEcoCalculatorXPtr me, double a, double b) {
    CCalculatorComponent* component;
    if (me == 0) return 0.0;
    component = ((CCalculatorXView*)me)->owner;
    if (component->included_subtractor != 0)
        return component->included_subtractor->pVTbl->Subtraction(component->included_subtractor, a, b);
    if (component->included_x != 0)
        return component->included_x->pVTbl->Subtraction(component->included_x, a, b);
    return a - b;
}
/* C delegates complete Y operations to D. D and E implement the Y
 * primitives; their X paths delegate to included/aggregated B respectively. */
static double ECOCALLMETHOD calculator_multiply(IEcoCalculatorYPtr me, double a, double b) {
    CCalculatorComponent* component;
    if (me == 0) return 0.0;
    component = ((CCalculatorYView*)me)->owner;
    if (component->included_y != 0)
        return component->included_y->pVTbl->Multiplication(component->included_y, a, b);
    return a * b;
}
static double ECOCALLMETHOD calculator_divide(IEcoCalculatorYPtr me, double a, double b) {
    CCalculatorComponent* component;
    if (me == 0 || b == 0.0) return 0.0;
    component = ((CCalculatorYView*)me)->owner;
    if (component->included_y != 0)
        return component->included_y->pVTbl->Division(component->included_y, a, b);
    return a / b;
}
static int16_t ECOCALLMETHOD calculator_try_divide(IEcoCalculatorYPtr me,
    double a, double b, double* result) {
    CCalculatorComponent* component;
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0.0;
    if (me == 0) return ERR_ECO_POINTER;
    if (b == 0.0) return ERR__POLYFIT_ARGUMENT;
    component = ((CCalculatorYView*)me)->owner;
    if (component->included_y != 0)
        return component->included_y->pVTbl->TryDivision(component->included_y, a, b, result);
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
static const UGUID* calculator_cid(uint32_t kind) {
    if (kind == CALCULATOR_A) return &CID_EcoCalculatorA;
    if (kind == CALCULATOR_B) return &CID_EcoCalculatorB;
    if (kind == CALCULATOR_C) return &CID_EcoCalculatorC;
    if (kind == CALCULATOR_D) return &CID_EcoCalculatorD;
    return &CID_EcoCalculatorE;
}
static int16_t calculator_child(IEcoMemoryAllocator1* memory,
    IEcoInterfaceBus1* bus, uint32_t kind, IEcoUnknown* outer,
    const UGUID* iid, voidptr_t* result) {
    CCalculatorComponent* child; int16_t status;
    *result = 0;
    if (bus != 0)
        return bus->pVTbl->QueryComponent(bus, calculator_cid(kind), outer, iid, result);
    status = Calculator_CreateOnBus(memory, kind, outer, 0, &child);
    if (status != ERR_ECO_SUCCESS) return status;
    status = child->inner_unknown.iface.pVTbl->QueryInterface(
        &child->inner_unknown.iface, iid, result);
    child->inner_unknown.iface.pVTbl->Release(&child->inner_unknown.iface);
    return status;
}
int16_t Calculator_CreateOnBus(IEcoMemoryAllocator1* memory, uint32_t kind,
    IEcoUnknown* outer, IEcoInterfaceBus1* bus, CCalculatorComponent** result) {
    CCalculatorComponent* component; int16_t status;
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0;
    if (memory == 0 || kind > CALCULATOR_E) return ERR__POLYFIT_ARGUMENT;
    component = (CCalculatorComponent*)memory->pVTbl->Alloc(memory,
        (uint32_t)sizeof(CCalculatorComponent));
    if (component == 0) return ERR_ECO_OUTOFMEMORY;
    component->inner_unknown.iface.pVTbl = &calculator_inner_vtbl;
    component->inner_unknown.owner = component;
    component->x.iface.pVTbl = &calculator_x_vtbl;
    component->y.iface.pVTbl = &calculator_y_vtbl;
    component->x.owner = component; component->y.owner = component;
    component->references = 1u; component->kind = kind;
    component->memory = memory; component->outer = outer;
    component->included_x = 0; component->included_y = 0;
    component->included_subtractor = 0;
    component->aggregated_inner = 0; component->aggregated_x = 0;
    memory->pVTbl->AddRef(memory);
    status = ERR_ECO_SUCCESS;
    if (kind == CALCULATOR_C)
        status = calculator_child(memory, bus, CALCULATOR_A, 0,
            &IID_IEcoCalculatorX, (voidptr_t*)&component->included_x);
    if (status == ERR_ECO_SUCCESS && kind == CALCULATOR_C)
        status = calculator_child(memory, bus, CALCULATOR_D, 0,
            &IID_IEcoCalculatorY, (voidptr_t*)&component->included_y);
    if (status == ERR_ECO_SUCCESS && kind == CALCULATOR_C)
        status = component->included_y->pVTbl->QueryInterface(component->included_y,
            &IID_IEcoCalculatorX, (voidptr_t*)&component->included_subtractor);
    if (kind == CALCULATOR_D)
        status = calculator_child(memory, bus, CALCULATOR_B, 0,
            &IID_IEcoCalculatorX, (voidptr_t*)&component->included_x);
    if (kind == CALCULATOR_E) {
        status = calculator_child(memory, bus, CALCULATOR_B,
            calculator_controller(component), &IID_IEcoUnknown,
            (voidptr_t*)&component->aggregated_inner);
        if (status == ERR_ECO_SUCCESS) {
            status = component->aggregated_inner->pVTbl->QueryInterface(
                component->aggregated_inner, &IID_IEcoCalculatorX,
                (voidptr_t*)&component->aggregated_x);
            if (status == ERR_ECO_SUCCESS)
                component->aggregated_x->pVTbl->Release(component->aggregated_x);
        }
    }
    if (status != ERR_ECO_SUCCESS) {
        calculator_inner_release(&component->inner_unknown.iface);
        return status;
    }
    *result = component;
    return ERR_ECO_SUCCESS;
}
int16_t Calculator_Create(IEcoMemoryAllocator1* memory, uint32_t kind,
    IEcoUnknown* outer, CCalculatorComponent** result) {
    return Calculator_CreateOnBus(memory, kind, outer, 0, result);
}

typedef struct CCalculatorFactory {
    IEcoComponentFactory factory;
    uint32_t references;
    uint32_t kind;
    char_t* name;
} CCalculatorFactory;

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
    if (outer != 0 && !guid_equal(riid, &IID_IEcoUnknown)) return ERR_ECO_NOAGGREGATION;
    factory = (CCalculatorFactory*)me; bus = 0; memory = 0; component = 0;
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&bus);
    if (status != ERR_ECO_SUCCESS || bus == 0) return ERR_ECO_GET_MEMORY_ALLOCATOR;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoMemoryManager1, 0,
        &IID_IEcoMemoryAllocator1, (voidptr_t*)&memory);
    if (status != ERR_ECO_SUCCESS || memory == 0) {
        bus->pVTbl->Release(bus);
        return ERR_ECO_GET_MEMORY_ALLOCATOR;
    }
    status = Calculator_CreateOnBus(memory, factory->kind, outer, bus, &component);
    bus->pVTbl->Release(bus);
    memory->pVTbl->Release(memory);
    if (status != ERR_ECO_SUCCESS) return status;
    /* Factory ownership is always the private, nondelegating reference. */
    status = component->inner_unknown.iface.pVTbl->QueryInterface(
        &component->inner_unknown.iface, riid, ppv);
    component->inner_unknown.iface.pVTbl->Release(&component->inner_unknown.iface);
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
    if (me == 0) return ""; return "1.0.0";
}
static char_t* ECOCALLMETHOD factory_manufacturer(IEcoComponentFactoryPtr_t me) {
    if (me == 0) return ""; return "Laboratory 2";
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
