/*
 * Кодировка: UTF-8.
 * Назначение: консольный Eco UnitTest лабораторной работы №1, задание 44.
 * Описание: проверяет обе реализации polyfit через IEcoInterfaceBus1.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#include <stdio.h>
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "polyfit_bus.h"

static uint32_t checks;
static uint32_t failures;

static void check(int condition) { ++checks; if (!condition) ++failures; }
static int close_to(double actual, double expected, double tolerance) {
    double delta = actual - expected;
    if (delta < 0.0) delta = -delta;
    return delta <= tolerance;
}
static int close_float(float actual, float expected, float tolerance) {
    float delta = actual - expected;
    if (delta < 0.0f) delta = -delta;
    return delta <= tolerance;
}
static int close_long_double(long double actual, long double expected,
    long double tolerance) {
    long double delta = actual - expected;
    if (delta < 0.0L) delta = -delta;
    return delta <= tolerance;
}

static void check_polynomial(IEcoPolynomialMathPtr component) {
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
}

static void check_constant_and_scale(IEcoPolynomialMathPtr component) {
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

static void check_overloads(IEcoPolynomialMathPtr component) {
    const float xf[5] = {-2.0f, -1.0f, 0.0f, 1.0f, 2.0f};
    const float yf[5] = {11.0f, 5.0f, 3.0f, 5.0f, 11.0f};
    const int xi[5] = {-2, -1, 0, 1, 2};
    const int yi[5] = {11, 5, 3, 5, 11};
    const long xl[5] = {-2L, -1L, 0L, 1L, 2L};
    const long yl[5] = {11L, 5L, 3L, 5L, 11L};
    const long double xld[5] = {-2.0L, -1.0L, 0.0L, 1.0L, 2.0L};
    const long double yld[5] = {11.0L, 5.0L, 3.0L, 5.0L, 11.0L};
    float cf[3]; double ci[3]; long double cl[3]; int16_t status;
    status = component->pVTbl->polyfitf(component, xf, yf, 5u, 2u, cf);
    check(status == ERR_ECO_SUCCESS); check(close_float(cf[0], 2.0f, 1.0e-4f));
    status = component->pVTbl->polyfiti(component, xi, yi, 5u, 2u, ci);
    check(status == ERR_ECO_SUCCESS); check(close_to(ci[2], 3.0, 1.0e-9));
    status = component->pVTbl->polyfit_long(component, xl, yl, 5u, 2u, cl);
    check(status == ERR_ECO_SUCCESS); check(close_long_double(cl[0], 2.0L, 1.0e-9L));
    status = component->pVTbl->polyfitl(component, xld, yld, 5u, 2u, cl);
    check(status == ERR_ECO_SUCCESS); check(close_long_double(cl[2], 3.0L, 1.0e-9L));
}

static void check_services(IEcoPolynomialMathPtr component) {
    const double x[5] = {-2.0, -1.0, 0.0, 1.0, 2.0};
    const double y[5] = {11.0, 5.0, 3.0, 5.0, 11.0};
    EcoVectorView vx; EcoVectorView vy; IEcoPolynomial* polynomial;
    IEcoPolynomial* queried; IEcoPolynomialMath* math; voidptr_t unknown;
    double coefficients[3]; double value; uint32_t degree; int16_t status;
    vx.data = x; vx.length = 5u; vy.data = y; vy.length = 5u;
    status = component->pVTbl->FitVector(component, &vx, &vy, 2u, coefficients);
    check(status == ERR_ECO_SUCCESS); check(close_to(coefficients[2], 3.0, 1.0e-9));
    status = component->pVTbl->Add(component, 2.0, 3.0, &value);
    check(status == ERR_ECO_SUCCESS && close_to(value, 5.0, 1.0e-12));
    status = component->pVTbl->Subtract(component, 7.0, 4.0, &value);
    check(status == ERR_ECO_SUCCESS && close_to(value, 3.0, 1.0e-12));
    polynomial = 0;
    status = component->pVTbl->new_Polynomial(component, x, y, 5u, 2u, &polynomial);
    check(status == ERR_ECO_SUCCESS && polynomial != 0);
    if (polynomial != 0) {
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
        polynomial->pVTbl->Release(polynomial);
    }
    math = 0;
    status = component->pVTbl->QueryInterface(component, &IID_IEcoPolynomialMath,
        (voidptr_t*)&math);
    check(status == ERR_ECO_SUCCESS && math != 0);
    if (math != 0) math->pVTbl->Release(math);
    unknown = 0;
    status = component->pVTbl->QueryInterface(component, &IID_IEcoPolynomial,
        &unknown);
    check(status == ERR_ECO_NOINTERFACE && unknown == 0);
}

static void check_errors(IEcoPolynomialMathPtr component) {
    const double x[5] = {-2.0, -1.0, 0.0, 1.0, 2.0};
    double y[5] = {11.0, 5.0, 3.0, 5.0, 11.0};
    double coefficients[3]; volatile double zero; int16_t status;
    status = component->pVTbl->polyfit(component, 0, y, 5u, 2u, coefficients);
    check(status == ERR_ECO_POINTER);
    status = component->pVTbl->polyfit(component, x, y, 2u, 2u, coefficients);
    check(status == ERR__POLYFIT_ARGUMENT);
    status = component->pVTbl->polyfit(component, x, y, 5u, 17u, coefficients);
    check(status == ERR__POLYFIT_ARGUMENT);
    status = component->pVTbl->polyfit(component, x, y, 5u, 2u, 0);
    check(status == ERR_ECO_POINTER);
    zero = 0.0; y[0] = y[0] / zero;
    status = component->pVTbl->polyfit(component, x, y, 5u, 2u, coefficients);
    check(status == ERR__POLYFIT_NONFINITE);
}

static void verify_component(IEcoPolynomialMathPtr component) {
    check(component != 0); if (component == 0) return;
    check_polynomial(component); check_constant_and_scale(component);
    check_overloads(component); check_services(component); check_errors(component);
    check(component->pVTbl->AddRef(component) > 1u);
    check(component->pVTbl->Release(component) > 0u);
}

int16_t EcoMain(IEcoUnknown* unknown) {
    IEcoSystem1* system; IEcoInterfaceBus1* bus; IEcoPolynomialMathPtr component;
    const UGUID* ids[2]; uint32_t i; int16_t status;
    system = 0; bus = 0; component = 0;
    ids[0] = &CID_EcoPolynomialMath; ids[1] = &CID_EcoPolynomialMathQR;
    if (unknown == 0) return ERR_ECO_POINTER;
    status = unknown->pVTbl->QueryInterface(unknown, &GID_IEcoSystem,
        (voidptr_t*)&system);
    if (status != ERR_ECO_SUCCESS || system == 0) return status;
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&bus);
    if (status != ERR_ECO_SUCCESS || bus == 0) {
        system->pVTbl->Release(system); return status;
    }
#ifdef ECO_LIB
    status = PolyfitComponent_RegisterOnBus(bus);
    if (status != ERR_ECO_SUCCESS) {
        bus->pVTbl->Release(bus); system->pVTbl->Release(system); return status;
    }
#endif
    for (i = 0u; i < 2u; ++i) {
        component = 0;
        status = bus->pVTbl->QueryComponent(bus, ids[i], 0,
            &IID_IEcoPolynomialMath, (voidptr_t*)&component);
        check(status == ERR_ECO_SUCCESS && component != 0);
        if (component != 0) { verify_component(component); component->pVTbl->Release(component); }
    }
#ifdef ECO_LIB
    bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMathQR);
    bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMath);
#endif
    bus->pVTbl->Release(bus); system->pVTbl->Release(system);
    printf("Проверок: %lu, ошибок: %lu\n", (unsigned long)checks,
        (unsigned long)failures);
    return failures == 0u ? ERR_ECO_SUCCESS : ERR__POLYFIT_ARGUMENT;
}

