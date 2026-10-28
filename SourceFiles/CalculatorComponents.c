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
    if (guid_equal(riid, &IID_IEcoCalculatorSafe) && component->kind >= CALCULATOR_C) {
        calculator_add_ref(component); *ppv = &component->safe.iface;
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
/* The standard ABI uses int16 operands. Promote before arithmetic. */
static int32_t ECOCALLMETHOD calculator_add(IEcoCalculatorXPtr me, int16_t a, int16_t b) {
    CCalculatorComponent* component;
    if (me == 0) return 0;
    component = ((CCalculatorXView*)me)->owner;
    if (component->included_x != 0)
        return component->included_x->pVTbl->Addition(component->included_x, a, b);
    return (int32_t)a + (int32_t)b;
}
static int16_t ECOCALLMETHOD calculator_subtract(IEcoCalculatorXPtr me, int16_t a, int16_t b) {
    CCalculatorComponent* component; int32_t value;
    if (me == 0) return 0;
    component = ((CCalculatorXView*)me)->owner;
    if (component->included_x != 0)
        return component->included_x->pVTbl->Subtraction(component->included_x, a, b);
    value = (int32_t)a - (int32_t)b;
    /* Define the educational int16 wrap without out-of-range conversion. */
    if (value > 32767L) value -= 65536L;
    if (value < -32768L) value += 65536L;
    return (int16_t)value;
}
static int32_t ECOCALLMETHOD calculator_multiply(IEcoCalculatorYPtr me, int16_t a, int16_t b) {
    if (me == 0) return 0;
    return (int32_t)a * (int32_t)b;
}
static int16_t checked_division(int16_t a, int16_t b, int16_t* result) {
    int32_t left, right;
    if (b == 0) return ERR_ECO_CALCULATOR_ZERO;
    if (a == -32768L && b == -1) return ERR_ECO_CALCULATOR_OVERFLOW;
    /* Explicit truncation toward zero, including on pre-C99 compilers. */
    left = a; right = b;
    if (left < 0) left = -left;
    if (right < 0) right = -right;
    left /= right;
    if ((a < 0) != (b < 0)) left = -left;
    *result = (int16_t)left;
    return ERR_ECO_SUCCESS;
}
static int16_t ECOCALLMETHOD calculator_divide(IEcoCalculatorYPtr me, int16_t a, int16_t b) {
    int16_t result;
    if (me == 0) return 0;
    result = 0; checked_division(a, b, &result);
    return result;
}
static int16_t ECOCALLMETHOD calculator_safe_qi(IEcoCalculatorSafe* me,
    const UGUID* riid, voidptr_t* ppv) {
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    return calculator_public_qi(((CCalculatorSafeView*)me)->owner, riid, ppv);
}
static uint32_t ECOCALLMETHOD calculator_safe_add_ref(IEcoCalculatorSafe* me) {
    if (me == 0) return 0u;
    return calculator_add_ref(((CCalculatorSafeView*)me)->owner);
}
static uint32_t ECOCALLMETHOD calculator_safe_release(IEcoCalculatorSafe* me) {
    if (me == 0) return 0u;
    return calculator_release(((CCalculatorSafeView*)me)->owner);
}
static int16_t ECOCALLMETHOD calculator_try_divide(IEcoCalculatorSafe* me,
    int16_t a, int16_t b, int16_t* result) {
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0;
    if (me == 0) return ERR_ECO_POINTER;
    return checked_division(a, b, result);
}
static IEcoCalculatorXVTbl calculator_x_vtbl = {
    calculator_x_qi, calculator_x_add_ref, calculator_x_release,
    calculator_add, calculator_subtract
};
static IEcoCalculatorYVTbl calculator_y_vtbl = {
    calculator_y_qi, calculator_y_add_ref, calculator_y_release,
    calculator_multiply, calculator_divide
};
static IEcoCalculatorSafeVTbl calculator_safe_vtbl = {
    calculator_safe_qi, calculator_safe_add_ref, calculator_safe_release,
    calculator_try_divide
};
static int16_t calculator_create(IEcoMemoryAllocator1*, uint32_t,
    IEcoUnknown*, IEcoInterfaceBus1*, CCalculatorComponent**);
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
    status = calculator_create(memory, kind, outer, 0, &child);
    if (status != ERR_ECO_SUCCESS) return status;
    status = child->inner_unknown.iface.pVTbl->QueryInterface(
        &child->inner_unknown.iface, iid, result);
    child->inner_unknown.iface.pVTbl->Release(&child->inner_unknown.iface);
    return status;
}
static int16_t calculator_create(IEcoMemoryAllocator1* memory, uint32_t kind,
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
    component->safe.iface.pVTbl = &calculator_safe_vtbl;
    component->safe.owner = component;
    component->x.owner = component; component->y.owner = component;
    component->references = 1u; component->kind = kind;
    component->memory = memory; component->outer = outer;
    component->included_x = 0; component->aggregated_inner = 0;
    memory->pVTbl->AddRef(memory);
    status = ERR_ECO_SUCCESS;
    /* A and C are independent implementations of the same component contract. */
    if (kind == CALCULATOR_D) {
        status = calculator_child(memory, bus, CALCULATOR_B, 0,
            &IID_IEcoCalculatorX, (voidptr_t*)&component->included_x);
        if (status == ERR_ECO_COMPONENT_NOTFOUND || status == ERR_ECO_NOINTERFACE)
            status = calculator_child(memory, bus, CALCULATOR_A, 0,
                &IID_IEcoCalculatorX, (voidptr_t*)&component->included_x);
    }
    if (kind == CALCULATOR_E)
        status = calculator_child(memory, bus, CALCULATOR_B,
            calculator_controller(component), &IID_IEcoUnknown,
            (voidptr_t*)&component->aggregated_inner);
    if (status != ERR_ECO_SUCCESS) {
        calculator_inner_release(&component->inner_unknown.iface);
        return status;
    }
    *result = component;
    return ERR_ECO_SUCCESS;
}
#ifndef ECO_CALCULATOR_KIND
int16_t Calculator_Create(IEcoMemoryAllocator1* memory, uint32_t kind,
    IEcoUnknown* outer, CCalculatorComponent** result) {
    return calculator_create(memory, kind, outer, 0, result);
}
int16_t Calculator_CreateOnBus(IEcoMemoryAllocator1* memory, uint32_t kind,
    IEcoUnknown* outer, IEcoInterfaceBus1* bus, CCalculatorComponent** result) {
    return calculator_create(memory, kind, outer, bus, result);
}
#endif

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
    status = calculator_create(memory, factory->kind, outer, bus, &component);
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
    if (me == 0) return ""; return "2.0.0";
}
static char_t* ECOCALLMETHOD factory_manufacturer(IEcoComponentFactoryPtr_t me) {
    if (me == 0) return ""; return "Laboratory 2";
}
static IEcoComponentFactoryVTbl factory_vtbl = {
    factory_qi, factory_add_ref, factory_release, factory_alloc,
    factory_init, factory_name, factory_version, factory_manufacturer
};
#if !defined(ECO_CALCULATOR_KIND) || ECO_CALCULATOR_KIND == 0
static CCalculatorFactory factory_a = {{&factory_vtbl},1u,CALCULATOR_A,"CalculatorA"};
IEcoComponentFactory* GetCalculatorAFactory(void) { return &factory_a.factory; }
#endif
#if !defined(ECO_CALCULATOR_KIND) || ECO_CALCULATOR_KIND == 1
static CCalculatorFactory factory_b = {{&factory_vtbl},1u,CALCULATOR_B,"CalculatorB"};
IEcoComponentFactory* GetCalculatorBFactory(void) { return &factory_b.factory; }
#endif
#if !defined(ECO_CALCULATOR_KIND) || ECO_CALCULATOR_KIND == 2
static CCalculatorFactory factory_c = {{&factory_vtbl},1u,CALCULATOR_C,"CalculatorC"};
IEcoComponentFactory* GetCalculatorCFactory(void) { return &factory_c.factory; }
#endif
#if !defined(ECO_CALCULATOR_KIND) || ECO_CALCULATOR_KIND == 3
static CCalculatorFactory factory_d = {{&factory_vtbl},1u,CALCULATOR_D,"CalculatorD"};
IEcoComponentFactory* GetCalculatorDFactory(void) { return &factory_d.factory; }
#endif
#if !defined(ECO_CALCULATOR_KIND) || ECO_CALCULATOR_KIND == 4
static CCalculatorFactory factory_e = {{&factory_vtbl},1u,CALCULATOR_E,"CalculatorE"};
IEcoComponentFactory* GetCalculatorEFactory(void) { return &factory_e.factory; }
#endif
#if defined(ECO_CALCULATOR_KIND) && ECO_CALCULATOR_KIND == 2
IEcoComponentFactory* GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E = &factory_c.factory;
#elif !defined(ECO_CALCULATOR_KIND) || ECO_CALCULATOR_KIND == 0
#ifdef ECO_REPLACE_A_WITH_C
IEcoComponentFactory* GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E = &factory_c.factory;
#else
IEcoComponentFactory* GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E = &factory_a.factory;
#endif
#endif
#if !defined(ECO_CALCULATOR_KIND) || ECO_CALCULATOR_KIND == 1
IEcoComponentFactory* GetIEcoComponentFactoryPtr_AE202E543CE54550899603BD70C62565 = &factory_b.factory;
#endif
#if !defined(ECO_CALCULATOR_KIND) || ECO_CALCULATOR_KIND == 3
IEcoComponentFactory* GetIEcoComponentFactoryPtr_3A8E44677E82475CB4A3719ED8397E61 = &factory_d.factory;
#endif
#if !defined(ECO_CALCULATOR_KIND) || ECO_CALCULATOR_KIND == 4
IEcoComponentFactory* GetIEcoComponentFactoryPtr_872FEF1DE3314B87AD44D1E7C232C2F0 = &factory_e.factory;
#endif
#if defined(ECO_DLL) && defined(ECO_CALCULATOR_KIND)
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr(void) {
#if ECO_CALCULATOR_KIND == 0
    return GetCalculatorAFactory();
#elif ECO_CALCULATOR_KIND == 1
    return GetCalculatorBFactory();
#elif ECO_CALCULATOR_KIND == 2
    return GetCalculatorCFactory();
#elif ECO_CALCULATOR_KIND == 3
    return GetCalculatorDFactory();
#elif ECO_CALCULATOR_KIND == 4
    return GetCalculatorEFactory();
#else
#error Unsupported ECO_CALCULATOR_KIND
#endif
}
#endif
