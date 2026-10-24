/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#include <stdio.h>
#include "polyfit_component.h"
#include "IEcoPolynomial.h"
#include "IdEcoPolynomialMath.h"
#include "polyfit_bus.h"

typedef struct MockAllocator {
    IEcoMemoryAllocator1 allocator;
    IEcoMemoryAllocator1VTbl vtbl;
    uint32_t references;
    uint32_t allocations;
    uint32_t frees;
    long double alignment;
    unsigned char storage[1048576];
    uint32_t offset;
} MockAllocator;

static int16_t ECOCALLMETHOD mem_qi(
    IEcoMemoryAllocator1* me, const UGUID* id, voidptr_t* out) {
    (void)me;
    (void)id;
    if (out == 0) return ERR_ECO_POINTER;
    *out = 0;
    return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD mem_add(IEcoMemoryAllocator1* me) {
    MockAllocator* memory;
    if (me == 0) return 0u;
    memory = (MockAllocator*)me;
    ++memory->references;
    return memory->references;
}
static uint32_t ECOCALLMETHOD mem_release(IEcoMemoryAllocator1* me) {
    MockAllocator* memory;
    if (me == 0) return 0u;
    memory = (MockAllocator*)me;
    if (memory->references != 0u) --memory->references;
    return memory->references;
}
static voidptr_t ECOCALLMETHOD mem_alloc(
    IEcoMemoryAllocator1* me, uint32_t size) {
    MockAllocator* memory;
    voidptr_t result;
    uint32_t aligned;
    if (me == 0) return 0;
    memory = (MockAllocator*)me;
    aligned = (memory->offset + 15u) & ~15u;
    if (aligned > 1048576u || size > 1048576u - aligned) return 0;
    result = (voidptr_t)(memory->storage + aligned);
    memory->offset = aligned + size;
    ++memory->allocations;
    return result;
}
static void ECOCALLMETHOD mem_free(IEcoMemoryAllocator1* me, voidptr_t value) {
    MockAllocator* memory;
    (void)value;
    if (me == 0) return;
    memory = (MockAllocator*)me;
    ++memory->frees;
}
static voidptr_t ECOCALLMETHOD mem_realloc(
    IEcoMemoryAllocator1* me, voidptr_t value, uint32_t size) {
    (void)value;
    return mem_alloc(me, size);
}
static voidptr_t ECOCALLMETHOD mem_copy(
    IEcoMemoryAllocator1* me, voidptr_t dst, voidptr_t src, uint32_t size) {
    unsigned char* target;
    unsigned char* source;
    uint32_t i;
    (void)me;
    target = (unsigned char*)dst;
    source = (unsigned char*)src;
    for (i = 0u; i < size; ++i) target[i] = source[i];
    return dst;
}
static voidptr_t ECOCALLMETHOD mem_fill(
    IEcoMemoryAllocator1* me, voidptr_t dst, char_t value, uint32_t size) {
    unsigned char* target;
    uint32_t i;
    (void)me;
    target = (unsigned char*)dst;
    for (i = 0u; i < size; ++i) target[i] = (unsigned char)value;
    return dst;
}
static int16_t ECOCALLMETHOD mem_compare(
    IEcoMemoryAllocator1* me, voidptr_t a, voidptr_t b, uint32_t size) {
    unsigned char* left;
    unsigned char* right;
    uint32_t i;
    (void)me;
    left = (unsigned char*)a;
    right = (unsigned char*)b;
    for (i = 0u; i < size; ++i) {
        if (left[i] != right[i]) return left[i] < right[i] ? -1 : 1;
    }
    return 0;
}
static uint32_t ECOCALLMETHOD mem_size(
    IEcoMemoryAllocator1* me, voidptr_t value) {
    (void)me;
    (void)value;
    return 0u;
}
static void allocator_init(MockAllocator* memory) {
    memory->vtbl.QueryInterface = mem_qi;
    memory->vtbl.AddRef = mem_add;
    memory->vtbl.Release = mem_release;
    memory->vtbl.Alloc = mem_alloc;
    memory->vtbl.Free = mem_free;
    memory->vtbl.Realloc = mem_realloc;
    memory->vtbl.Copy = mem_copy;
    memory->vtbl.Fill = mem_fill;
    memory->vtbl.Compare = mem_compare;
    memory->vtbl.RetrieveSize = mem_size;
    memory->allocator.pVTbl = &memory->vtbl;
    memory->references = 1u;
    memory->allocations = 0u;
    memory->frees = 0u;
    memory->offset = 0u;
    memory->alignment = 0.0L;
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

#define EXPECTED_CHECKS 72u
static uint32_t checks;
static uint32_t failures;
static void check(int condition) {
    ++checks;
    if (!condition) ++failures;
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
    printf("COMPONENT degree=2 coefficients=[%.17g, %.17g, %.17g]\n",
        coefficients[0], coefficients[1], coefficients[2]);
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
    printf("COMPONENT degree=3 coefficients=[%.17g, %.17g, %.17g, %.17g]\n",
        coefficients[0], coefficients[1], coefficients[2], coefficients[3]);
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
    status = component->pVTbl->polyfit(component, scaled_x, scaled_y,
        5u, 1u, coefficients);
    check(status == ERR_ECO_SUCCESS);
    check(close_to(coefficients[0], 2.0, 1.0e-6));
    check(close_to(coefficients[1], 3.0, 1.0e-3));
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
    check(status == ERR_ECO_SUCCESS && close_to(cf[0], 2.0, 1.0e-4));
    status = component->pVTbl->polyfiti(component, xi, yi, 5u, 2u, ci);
    check(status == ERR_ECO_SUCCESS && close_to(ci[2], 3.0, 1.0e-9));
    status = component->pVTbl->polyfit_long(component, xl, yl, 5u, 2u, cl);
    check(status == ERR_ECO_SUCCESS && close_long_double(cl[0], 2.0L, 1.0e-9L));
    status = component->pVTbl->polyfitl(component, xld, yld, 5u, 2u, cl);
    check(status == ERR_ECO_SUCCESS && close_long_double(cl[2], 3.0L, 1.0e-9L));
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
    component->pVTbl->AddRef(component);
    check(component->pVTbl->Release(component) != 0u);
}

int main(void) {
    MockAllocator memory;
    IEcoPolynomialMath* normal;
    IEcoPolynomialMath* qr;
    int16_t status;
    allocator_init(&memory);
    normal = 0;
    qr = 0;
    status = PolyfitComponent_Create_00000045(0, &memory.allocator, 0u, 0, &normal);
    check(status == ERR_ECO_SUCCESS);
    check_component(&memory, normal);
    status = PolyfitComponent_Create_00000045(0, &memory.allocator, 1u, 0, &qr);
    check(status == ERR_ECO_SUCCESS);
    check_component(&memory, qr);
    if (normal != 0) normal->pVTbl->Release(normal);
    if (qr != 0) qr->pVTbl->Release(qr);
    check(memory.allocations > 0u);
    check(memory.frees > 0u);
    printf("Проверок: %lu, ошибок: %lu\n", (unsigned long)checks,
        (unsigned long)failures);
    return failures == 0u && checks == EXPECTED_CHECKS ? 0 : 1;
}

