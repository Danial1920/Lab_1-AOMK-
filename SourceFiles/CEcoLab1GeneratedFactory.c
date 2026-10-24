/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#include "CEcoLab1Generated.h"
#include "CEcoLab1GeneratedFactory.h"
#include "IdEcoPolynomialMath.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoLab1.h"
#include "polyfit_core.h"
#include "IEcoInterfaceBus1.h"
#include "IdEcoMemoryManager1.h"
#if defined(ECO_LIB) || defined(ECO_DLL)
#include "IEcoSystem1.h"
#endif

#define POLYFIT_MAX_ORDER 17u

typedef struct CPolynomialData {
    IEcoPolynomial polynomial;
    uint32_t references;
    IEcoMemoryAllocator1* memory;
    uint32_t degree;
    double coefficients[POLYFIT_MAX_ORDER];
} CPolynomialData;

/* Component function implementation. */
static int guid_equal(const UGUID* a, const UGUID* b) {
    const byte_t* left;
    const byte_t* right;
    uint32_t i;
    if (a == 0 || b == 0) return 0;
    left = (const byte_t*)a;
    right = (const byte_t*)b;
    for (i = 0; i < 18u; ++i) {
        if (left[i] != right[i]) return 0;
    }
    return 1;
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD data_qi(
    IEcoPolynomial* me, const UGUID* riid, voidptr_t* ppv) {
    CPolynomialData* data;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    data = (CPolynomialData*)me;
    if (guid_equal(riid, &IID_IEcoPolynomial) ||
        guid_equal(riid, &IID_IEcoUnknown)) {
        ++data->references;
        *ppv = me;
        return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}

/* Component function implementation. */
static uint32_t ECOCALLMETHOD data_add_ref(IEcoPolynomial* me) {
    CPolynomialData* data;
    if (me == 0) return 0u;
    data = (CPolynomialData*)me;
    ++data->references;
    return data->references;
}

/* Component function implementation. */
static uint32_t ECOCALLMETHOD data_release(IEcoPolynomial* me) {
    CPolynomialData* data;
    IEcoMemoryAllocator1* memory;
    uint32_t references;
    if (me == 0) return 0u;
    data = (CPolynomialData*)me;
    if (data->references == 0u) return 0u;
    --data->references;
    references = data->references;
    if (references == 0u) {
        memory = data->memory;
        memory->pVTbl->Free(memory, data);
        memory->pVTbl->Release(memory);
    }
    return references;
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD data_degree(
    IEcoPolynomial* me, uint32_t* degree) {
    CPolynomialData* data;
    if (degree == 0) return ERR_ECO_POINTER;
    *degree = 0u;
    if (me == 0) return ERR_ECO_POINTER;
    data = (CPolynomialData*)me;
    *degree = data->degree;
    return ERR_ECO_SUCCESS;
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD data_coefficients(
    IEcoPolynomial* me, uint32_t capacity, double* coefficients) {
    CPolynomialData* data;
    uint32_t bytes;
    uint32_t capacity_bytes;
    if (me == 0) return ERR_ECO_POINTER;
    data = (CPolynomialData*)me;
    if (coefficients == 0) return ERR_ECO_POINTER;
    if (capacity != 0u && sizeof(double) > 0xFFFFFFFFu / capacity)
        return ERR_ECO_OUTOFMEMORY;
    capacity_bytes = (uint32_t)(sizeof(double) * capacity);
    data->memory->pVTbl->Fill(data->memory, coefficients, 0, capacity_bytes);
    if (capacity < data->degree + 1u) return ERR__POLYFIT_BUFFER;
    bytes = (uint32_t)(sizeof(double) * (data->degree + 1u));
    data->memory->pVTbl->Copy(data->memory, coefficients,
        data->coefficients, bytes);
    return ERR_ECO_SUCCESS;
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD data_evaluate(
    IEcoPolynomial* me, double x, double* value) {
    CPolynomialData* data;
    uint32_t i;
    if (value == 0) return ERR_ECO_POINTER;
    *value = 0.0;
    if (me == 0) return ERR_ECO_POINTER;
    data = (CPolynomialData*)me;
    *value = data->coefficients[0];
    for (i = 1u; i <= data->degree; ++i) {
        *value = *value * x + data->coefficients[i];
    }
    return ERR_ECO_SUCCESS;
}

static IEcoPolynomialVTbl data_vtbl = {
    data_qi, data_add_ref, data_release, data_degree,
    data_coefficients, data_evaluate
};

static uint32_t ECOCALLMETHOD component_add_ref(IEcoPolynomialMathPtr me);
static uint32_t ECOCALLMETHOD component_release(IEcoPolynomialMathPtr me);

/* Component function implementation. */
static int16_t ECOCALLMETHOD component_qi(
    IEcoPolynomialMathPtr me, const UGUID* riid, voidptr_t* ppv) {
    CPolynomialMath* component;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    component = (CPolynomialMath*)me;
    if (guid_equal(riid, &IID_IEcoUnknown) && component->m_pIOuter != 0)
        return component->m_pIOuter->pVTbl->QueryInterface(component->m_pIOuter,
            riid, ppv);
    if (guid_equal(riid, &IID_IEcoPolynomialMath) ||
        guid_equal(riid, &IID_IEcoLab1) ||
        guid_equal(riid, &IID_IEcoUnknown)) {
        component_add_ref(me);
        *ppv = me;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoCalculatorX)) {
        component_add_ref(me);
        *ppv = &component->calculator_x.iface;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoCalculatorY)) {
        component_add_ref(me);
        *ppv = &component->calculator_y.iface;
        return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}

/* Component function implementation. */
static uint32_t ECOCALLMETHOD component_add_ref(IEcoPolynomialMathPtr me) {
    CPolynomialMath* component;
    if (me == 0) return 0u;
    component = (CPolynomialMath*)me;
    if (component->m_pIOuter != 0)
        return component->m_pIOuter->pVTbl->AddRef(component->m_pIOuter);
    ++component->references;
    return component->references;
}

/* Component function implementation. */
static uint32_t ECOCALLMETHOD component_release(IEcoPolynomialMathPtr me) {
    CPolynomialMath* component;
    IEcoMemoryAllocator1* memory;
    uint32_t references;
    if (me == 0) return 0u;
    component = (CPolynomialMath*)me;
    if (component->m_pIOuter != 0)
        return component->m_pIOuter->pVTbl->Release(component->m_pIOuter);
    if (component->references == 0u) return 0u;
    --component->references;
    references = component->references;
    if (references == 0u) {
        memory = component->m_pIMem;
#if defined(ECO_LIB) || defined(ECO_DLL)
        if (component->m_pISys != 0) {
            component->m_pISys->pVTbl->Release(component->m_pISys);
        }
#endif
        memory->pVTbl->Free(memory, component);
        memory->pVTbl->Release(memory);
    }
    return references;
}

static int16_t ECOCALLMETHOD lab1_calc_x_qi(IEcoCalculatorXPtr me,
    const UGUID* riid, voidptr_t* ppv) {
    CPolynomialMath* component;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    component = ((CPolynomialXView*)me)->owner;
    if (guid_equal(riid, &IID_IEcoCalculatorX)) {
        component_add_ref(&component->math); *ppv = me; return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoCalculatorY)) {
        component_add_ref(&component->math); *ppv = &component->calculator_y.iface;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoPolynomialMath) ||
        guid_equal(riid, &IID_IEcoLab1)) {
        component_add_ref(&component->math); *ppv = &component->math;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoUnknown) && component->m_pIOuter != 0)
        return component->m_pIOuter->pVTbl->QueryInterface(component->m_pIOuter, riid, ppv);
    if (guid_equal(riid, &IID_IEcoUnknown)) {
        component_add_ref(&component->math); *ppv = &component->math;
        return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}
static int16_t ECOCALLMETHOD lab1_calc_y_qi(IEcoCalculatorYPtr me,
    const UGUID* riid, voidptr_t* ppv) {
    CPolynomialMath* component;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    component = ((CPolynomialYView*)me)->owner;
    if (guid_equal(riid, &IID_IEcoCalculatorY)) {
        component_add_ref(&component->math); *ppv = me; return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoCalculatorX)) {
        component_add_ref(&component->math); *ppv = &component->calculator_x.iface;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoPolynomialMath) ||
        guid_equal(riid, &IID_IEcoLab1)) {
        component_add_ref(&component->math); *ppv = &component->math;
        return ERR_ECO_SUCCESS;
    }
    if (guid_equal(riid, &IID_IEcoUnknown) && component->m_pIOuter != 0)
        return component->m_pIOuter->pVTbl->QueryInterface(component->m_pIOuter, riid, ppv);
    if (guid_equal(riid, &IID_IEcoUnknown)) {
        component_add_ref(&component->math); *ppv = &component->math;
        return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD lab1_calc_x_add_ref(IEcoCalculatorXPtr me) {
    if (me == 0) return 0u;
    return component_add_ref(&((CPolynomialXView*)me)->owner->math);
}
static uint32_t ECOCALLMETHOD lab1_calc_x_release(IEcoCalculatorXPtr me) {
    if (me == 0) return 0u;
    return component_release(&((CPolynomialXView*)me)->owner->math);
}
static uint32_t ECOCALLMETHOD lab1_calc_y_add_ref(IEcoCalculatorYPtr me) {
    if (me == 0) return 0u;
    return component_add_ref(&((CPolynomialYView*)me)->owner->math);
}
static uint32_t ECOCALLMETHOD lab1_calc_y_release(IEcoCalculatorYPtr me) {
    if (me == 0) return 0u;
    return component_release(&((CPolynomialYView*)me)->owner->math);
}
static double ECOCALLMETHOD lab1_add(IEcoCalculatorXPtr me, double a, double b) {
    if (me == 0) return 0.0; return a + b;
}
static double ECOCALLMETHOD lab1_subtract(IEcoCalculatorXPtr me, double a, double b) {
    if (me == 0) return 0.0; return a - b;
}
static double ECOCALLMETHOD lab1_multiply(IEcoCalculatorYPtr me, double a, double b) {
    if (me == 0) return 0.0; return a * b;
}
static double ECOCALLMETHOD lab1_divide(IEcoCalculatorYPtr me, double a, double b) {
    if (me == 0 || b == 0.0) return 0.0; return a / b;
}
static int16_t ECOCALLMETHOD lab1_try_divide(IEcoCalculatorYPtr me,
    double a, double b, double* result) {
    if (result == 0) return ERR_ECO_POINTER;
    *result = 0.0;
    if (me == 0) return ERR_ECO_POINTER;
    if (b == 0.0) return ERR__POLYFIT_ARGUMENT;
    *result = a / b; return ERR_ECO_SUCCESS;
}
static IEcoCalculatorXVTbl lab1_x_vtbl = {
    lab1_calc_x_qi, lab1_calc_x_add_ref, lab1_calc_x_release,
    lab1_add, lab1_subtract
};
static IEcoCalculatorYVTbl lab1_y_vtbl = {
    lab1_calc_y_qi, lab1_calc_y_add_ref, lab1_calc_y_release,
    lab1_multiply, lab1_divide, lab1_try_divide
};

/* Generate one ACOM method for a typed input/output pair. */
#define COMPONENT_POLYFIT(NAME, TYPE, OUT, SOLVER) \
static int16_t ECOCALLMETHOD NAME(IEcoPolynomialMathPtr me, \
    const TYPE* x, const TYPE* y, uint32_t count, uint32_t degree, \
    OUT* coefficients) { \
    CPolynomialMath* component; \
    uint32_t bytes; \
    if (me == 0) return ERR_ECO_POINTER; \
    component = (CPolynomialMath*)me; \
    if (coefficients == 0) return ERR_ECO_POINTER; \
    if (degree >= POLYFIT_MAX_ORDER) return ERR__POLYFIT_ARGUMENT; \
    if (sizeof(OUT) > 0xFFFFFFFFu / (degree + 1u)) \
        return ERR_ECO_OUTOFMEMORY; \
    bytes = (uint32_t)(sizeof(OUT) * (degree + 1u)); \
    component->m_pIMem->pVTbl->Fill(component->m_pIMem, coefficients, 0, bytes); \
    if (x == 0 || y == 0) return ERR_ECO_POINTER; \
    return SOLVER(component->m_pIMem, x, y, count, degree, coefficients, \
        component->method); \
}

COMPONENT_POLYFIT(component_polyfit, double, double, polyfit_solve_00000045)
COMPONENT_POLYFIT(component_polyfitf, float, float, polyfit_solve_float_00000045)
COMPONENT_POLYFIT(component_polyfitl, long double, long double,
    polyfit_solve_long_double_00000045)
COMPONENT_POLYFIT(component_polyfiti, int, double, polyfit_solve_int_00000045)
COMPONENT_POLYFIT(component_polyfit_long, long, long double,
    polyfit_solve_long_00000045)

/* Component function implementation. */
static int16_t ECOCALLMETHOD component_add(
    IEcoPolynomialMathPtr me, double x, double y, double* value) {
    CPolynomialMath* component;
    if (value == 0) return ERR_ECO_POINTER;
    *value = 0.0;
    if (me == 0) return ERR_ECO_POINTER;
    component = (CPolynomialMath*)me;
    (void)component;
    *value = x + y;
    return ERR_ECO_SUCCESS;
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD component_subtract(
    IEcoPolynomialMathPtr me, double x, double y, double* value) {
    CPolynomialMath* component;
    if (value == 0) return ERR_ECO_POINTER;
    *value = 0.0;
    if (me == 0) return ERR_ECO_POINTER;
    component = (CPolynomialMath*)me;
    (void)component;
    *value = x - y;
    return ERR_ECO_SUCCESS;
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD component_fit_vector(
    IEcoPolynomialMathPtr me, const EcoVectorView* x, const EcoVectorView* y,
    uint32_t degree, double* coefficients) {
    CPolynomialMath* component;
    uint32_t bytes;
    if (me == 0) return ERR_ECO_POINTER;
    component = (CPolynomialMath*)me;
    if (coefficients == 0) return ERR_ECO_POINTER;
    if (degree >= POLYFIT_MAX_ORDER) return ERR__POLYFIT_ARGUMENT;
    if (sizeof(double) > 0xFFFFFFFFu / (degree + 1u))
        return ERR_ECO_OUTOFMEMORY;
    bytes = (uint32_t)(sizeof(double) * (degree + 1u));
    component->m_pIMem->pVTbl->Fill(component->m_pIMem, coefficients, 0, bytes);
    if (x == 0 || y == 0 || x->data == 0 || y->data == 0 ||
        x->length != y->length) return ERR__POLYFIT_ARGUMENT;
    return component_polyfit(me, x->data, y->data, x->length,
        degree, coefficients);
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD component_new_polynomial(
    IEcoPolynomialMathPtr me, const double* x, const double* y,
    uint32_t count, uint32_t degree, IEcoPolynomial** polynomial) {
    CPolynomialMath* component;
    CPolynomialData* data;
    int16_t status;
    if (me == 0) return ERR_ECO_POINTER;
    component = (CPolynomialMath*)me;
    if (polynomial == 0) return ERR_ECO_POINTER;
    *polynomial = 0;
    if (x == 0 || y == 0) return ERR_ECO_POINTER;
    if (degree >= POLYFIT_MAX_ORDER) return ERR__POLYFIT_ARGUMENT;
    data = (CPolynomialData*)component->m_pIMem->pVTbl->Alloc(
        component->m_pIMem, (uint32_t)sizeof(CPolynomialData));
    if (data == 0) return ERR_ECO_OUTOFMEMORY;
    data->polynomial.pVTbl = &data_vtbl;
    data->references = 1u;
    data->memory = component->m_pIMem;
    data->memory->pVTbl->AddRef(data->memory);
    data->degree = degree;
    status = component_polyfit(me, x, y, count, degree, data->coefficients);
    if (status != ERR_ECO_SUCCESS) {
        data->polynomial.pVTbl->Release(&data->polynomial);
        return status;
    }
    *polynomial = &data->polynomial;
    return ERR_ECO_SUCCESS;
}

static IEcoPolynomialMathVTbl component_vtbl = {
    component_qi, component_add_ref, component_release,
    component_polyfit, component_polyfitf, component_polyfitl,
    component_polyfiti, component_polyfit_long, component_add,
    component_subtract, component_fit_vector, component_new_polynomial
};

/* Component function implementation. */
int16_t PolyfitComponent_Create_00000045(
    IEcoUnknownPtr_t system, IEcoMemoryAllocator1* memory, uint32_t method,
    IEcoUnknownPtr_t outer, IEcoPolynomialMathPtr* component_out) {
    CPolynomialMath* component;
#if defined(ECO_LIB) || defined(ECO_DLL)
    int16_t status;
#endif
    if (component_out == 0) return ERR_ECO_POINTER;
    *component_out = 0;
    if (memory == 0 || method > 1u) return ERR__POLYFIT_ARGUMENT;
    component = (CPolynomialMath*)memory->pVTbl->Alloc(
        memory, (uint32_t)sizeof(CPolynomialMath));
    if (component == 0) return ERR_ECO_OUTOFMEMORY;
    component->math.pVTbl = &component_vtbl;
    component->calculator_x.iface.pVTbl = &lab1_x_vtbl;
    component->calculator_y.iface.pVTbl = &lab1_y_vtbl;
    component->calculator_x.owner = component;
    component->calculator_y.owner = component;
    component->references = 1u;
    component->method = method;
    component->m_pIMem = memory;
    component->m_pISys = 0;
    component->m_pIOuter = outer;
    memory->pVTbl->AddRef(memory);
    if (system != 0) {
#if defined(ECO_LIB) || defined(ECO_DLL)
        status = system->pVTbl->QueryInterface(system, &GID_IEcoSystem,
            (voidptr_t*)&component->m_pISys);
        if (status != ERR_ECO_SUCCESS || component->m_pISys == 0) {
            memory->pVTbl->Free(memory, component);
            memory->pVTbl->Release(memory);
            return ERR_ECO_NOSYSTEM;
        }
#else
        (void)system;
#endif
    }
    *component_out = &component->math;
    return ERR_ECO_SUCCESS;
}

void PolyfitComponent_Destroy_00000045(IEcoPolynomialMathPtr value) {
    CPolynomialMath* component;
    IEcoMemoryAllocator1* memory;
    if (value == 0) return;
    component = (CPolynomialMath*)value;
    memory = component->m_pIMem;
#if defined(ECO_LIB) || defined(ECO_DLL)
    if (component->m_pISys != 0)
        component->m_pISys->pVTbl->Release(component->m_pISys);
#endif
    memory->pVTbl->Free(memory, component);
    memory->pVTbl->Release(memory);
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD factory_qi(
    IEcoComponentFactoryPtr_t me, const UGUID* riid, voidptr_t* ppv) {
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || riid == 0) return ERR_ECO_POINTER;
    if (guid_equal(riid, &IID_IEcoComponentFactory) ||
        guid_equal(riid, &IID_IEcoUnknown)) {
        me->pVTbl->AddRef(me);
        *ppv = me;
        return ERR_ECO_SUCCESS;
    }
    return ERR_ECO_NOINTERFACE;
}

/* Component function implementation. */
static uint32_t ECOCALLMETHOD factory_add_ref(IEcoComponentFactoryPtr_t me) {
    CPolynomialFactory* factory;
    if (me == 0) return 0u;
    factory = (CPolynomialFactory*)me;
    ++factory->references;
    return factory->references;
}

/* Component function implementation. */
static uint32_t ECOCALLMETHOD factory_release(IEcoComponentFactoryPtr_t me) {
    CPolynomialFactory* factory;
    if (me == 0) return 0u;
    factory = (CPolynomialFactory*)me;
    if (factory->references != 0u) --factory->references;
    return factory->references;
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD factory_alloc(
    IEcoComponentFactoryPtr_t me, IEcoUnknownPtr_t system,
    IEcoUnknownPtr_t outer, const UGUID* riid, voidptr_t* ppv) {
    CPolynomialFactory* factory;
    IEcoInterfaceBus1* bus;
    IEcoMemoryAllocator1* memory;
    IEcoPolynomialMathPtr component;
    int16_t status;
    if (ppv == 0) return ERR_ECO_POINTER;
    *ppv = 0;
    if (me == 0 || system == 0 || riid == 0) return ERR_ECO_POINTER;
    factory = (CPolynomialFactory*)me;
    bus = 0;
    memory = 0;
    component = 0;
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&bus);
    if (status != ERR_ECO_SUCCESS || bus == 0) return ERR_ECO_GET_MEMORY_ALLOCATOR;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoMemoryManager1, 0,
        &IID_IEcoMemoryAllocator1, (voidptr_t*)&memory);
    bus->pVTbl->Release(bus);
    if (status != ERR_ECO_SUCCESS || memory == 0) return ERR_ECO_GET_MEMORY_ALLOCATOR;
    status = PolyfitComponent_Create_00000045(system, memory, factory->method,
        outer, &component);
    memory->pVTbl->Release(memory);
    if (status != ERR_ECO_SUCCESS) return status;
    status = component->pVTbl->QueryInterface(component, riid, ppv);
    if (outer == 0) component->pVTbl->Release(component);
    return status;
}

/* Component function implementation. */
static int16_t ECOCALLMETHOD factory_init(
    IEcoComponentFactoryPtr_t me, IEcoUnknownPtr_t system, voidptr_t data) {
    if (me == 0) return ERR_ECO_POINTER;
    (void)system;
    (void)data;
    return ERR_ECO_SUCCESS;
}

/* Component function implementation. */
static char_t* ECOCALLMETHOD factory_name(IEcoComponentFactoryPtr_t me) {
    (void)me;
    return "EcoPolynomialMath";
}

/* Component function implementation. */
static char_t* ECOCALLMETHOD factory_version(IEcoComponentFactoryPtr_t me) {
    (void)me;
    return "1.1.0";
}

/* Component function implementation. */
static char_t* ECOCALLMETHOD factory_manufacturer(IEcoComponentFactoryPtr_t me) {
    (void)me;
    return "Laboratory 1";
}

static IEcoComponentFactoryVTbl factory_vtbl = {
    factory_qi, factory_add_ref, factory_release, factory_alloc,
    factory_init, factory_name, factory_version, factory_manufacturer
};

static CPolynomialFactory normal_factory = {{&factory_vtbl}, 1u, 0u};
static CPolynomialFactory qr_factory = {{&factory_vtbl}, 1u, 1u};

/* Component function implementation. */
IEcoComponentFactory* GetPolynomialFactory_00000045(void) {
    return &normal_factory.factory;
}

/* Component function implementation. */
IEcoComponentFactory* GetPolynomialQRFactory_00000046(void) {
    return &qr_factory.factory;
}

#ifdef ECO_DLL
#ifdef GetIEcoComponentFactoryPtr
#undef GetIEcoComponentFactoryPtr
#endif
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr(void) {
#ifdef POLYFIT_QR_ONLY
    return &qr_factory.factory;
#else
    return &normal_factory.factory;
#endif
}
#endif

