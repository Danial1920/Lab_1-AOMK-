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
#include "IdEcoLab2.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoLab1.h"
#include "IEcoLab2.h"
#include "IdEcoMemoryManager1.h"

static uint32_t checks;
static uint32_t failures;
static uint32_t matrix_cases, factory_cases, allocation_cases, partial_cases, math_cases, lifetime_cases;

static void check_at(int condition, const char* file, int line) {
    ++checks; if (!condition) { ++failures; printf("FAIL %s:%d\n", file, line); }
}
#define check(c) check_at((c), __FILE__, __LINE__)
#include "RealAllocatorProbe.h"
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

static void check_qi_matrix(IEcoInterfaceBus1* bus, const UGUID* cid,
    const UGUID** supported, uint32_t count) {
    IEcoUnknown* root; IEcoUnknown* iface; IEcoUnknown* other; IEcoUnknown* identity;
    IEcoCalculatorX* x; IEcoCalculatorY* y;
    uint32_t i, j, added, released, refs, live; int16_t status; double result;
    refs = probe_reference_count(); live = real_memory.live; root = 0;
    status = bus->pVTbl->QueryComponent(bus, cid, 0, &IID_IEcoUnknown, (voidptr_t*)&root);
    check(status == ERR_ECO_SUCCESS && root != 0);
    if (root == 0) return;
    for (i = 0u; i < count; ++i) {
        iface = 0;
        status = root->pVTbl->QueryInterface(root, supported[i], (voidptr_t*)&iface);
        check(status == ERR_ECO_SUCCESS && iface != 0);
        if (iface == 0) continue;
        for (j = 0u; j < count; ++j) {
            other = 0; identity = 0;
            status = iface->pVTbl->QueryInterface(iface, supported[j], (voidptr_t*)&other);
            check(status == ERR_ECO_SUCCESS && other != 0);
            if (other != 0) {
                status = other->pVTbl->QueryInterface(other, &IID_IEcoUnknown, (voidptr_t*)&identity);
                check(status == ERR_ECO_SUCCESS && identity == root);
                if (identity != 0) identity->pVTbl->Release(identity);
                other->pVTbl->Release(other);
            }
        }
        other = (IEcoUnknown*)1;
        status = iface->pVTbl->QueryInterface(iface, &IID_IEcoPolynomial, (voidptr_t*)&other);
        check(status == ERR_ECO_NOINTERFACE && other == 0);
        other = (IEcoUnknown*)1;
        check(iface->pVTbl->QueryInterface(iface, 0, (voidptr_t*)&other) == ERR_ECO_POINTER && other == 0);
        check(iface->pVTbl->QueryInterface(iface, &IID_IEcoUnknown, 0) == ERR_ECO_POINTER);
        other = (IEcoUnknown*)1;
        check(iface->pVTbl->QueryInterface(0, &IID_IEcoUnknown, (voidptr_t*)&other) == ERR_ECO_POINTER && other == 0);
        added = iface->pVTbl->AddRef(iface); released = iface->pVTbl->Release(iface);
        check(added == released + 1u);
        iface->pVTbl->Release(iface);
    }
    x = 0; y = 0;
    status = root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorX, (voidptr_t*)&x);
    if (status == ERR_ECO_SUCCESS && x != 0) {
        check(x->pVTbl->Addition(x, 3.25, -1.5) == 1.75);
        check(x->pVTbl->Subtraction(x, 3.25, -1.5) == 4.75);
        check(x->pVTbl->Addition(x, 0.0, 0.0) == 0.0);
        check(x->pVTbl->Addition(0, 3.0, 5.0) == 0.0);
        x->pVTbl->Release(x);
    }
    status = root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorY, (voidptr_t*)&y);
    if (status == ERR_ECO_SUCCESS && y != 0) {
        check(y->pVTbl->Multiplication(y, -2.0, 4.5) == -9.0);
        check(y->pVTbl->Multiplication(y, 3.0, 0.0) == 0.0);
        check(y->pVTbl->Division(y, 7.5, -2.5) == -3.0);
        result = 9.0;
        check(y->pVTbl->TryDivision(y, 6.0, 3.0, &result) == ERR_ECO_SUCCESS && result == 2.0);
        result = 9.0;
        check(y->pVTbl->TryDivision(y, 1.0, 0.0, &result) == ERR__POLYFIT_ARGUMENT && result == 0.0);
        result = 9.0;
        check(y->pVTbl->TryDivision(0, 1.0, 2.0, &result) == ERR_ECO_POINTER && result == 0.0);
        check(y->pVTbl->TryDivision(y, 1.0, 2.0, 0) == ERR_ECO_POINTER);
        y->pVTbl->Release(y);
    }
    check(root->pVTbl->Release(root) == 0u);
    check(real_memory.live == live && probe_reference_count() == refs);
    ++matrix_cases;
}
static void check_all_components(IEcoInterfaceBus1* bus) {
    const UGUID* x_ids[2]; const UGUID* xy_ids[3];
    const UGUID* lab1_ids[5]; const UGUID* lab2_ids[6];
    x_ids[0] = &IID_IEcoUnknown; x_ids[1] = &IID_IEcoCalculatorX;
    xy_ids[0] = &IID_IEcoUnknown; xy_ids[1] = &IID_IEcoCalculatorX; xy_ids[2] = &IID_IEcoCalculatorY;
    lab1_ids[0] = &IID_IEcoUnknown; lab1_ids[1] = &IID_IEcoLab1;
    lab1_ids[2] = &IID_IEcoPolynomialMath; lab1_ids[3] = &IID_IEcoCalculatorX; lab1_ids[4] = &IID_IEcoCalculatorY;
    lab2_ids[0] = &IID_IEcoUnknown; lab2_ids[1] = &IID_IEcoLab2;
    lab2_ids[2] = &IID_IEcoLab1; lab2_ids[3] = &IID_IEcoPolynomialMath;
    lab2_ids[4] = &IID_IEcoCalculatorX; lab2_ids[5] = &IID_IEcoCalculatorY;
    check_qi_matrix(bus, &CID_EcoCalculatorA, x_ids, 2u);
    check_qi_matrix(bus, &CID_EcoCalculatorB, x_ids, 2u);
    check_qi_matrix(bus, &CID_EcoCalculatorC, xy_ids, 3u);
    check_qi_matrix(bus, &CID_EcoCalculatorD, xy_ids, 3u);
    check_qi_matrix(bus, &CID_EcoCalculatorE, xy_ids, 3u);
    check_qi_matrix(bus, &CID_EcoPolynomialMath, lab1_ids, 5u);
    check_qi_matrix(bus, &CID_EcoPolynomialMathQR, lab1_ids, 5u);
    check_qi_matrix(bus, &CID_EcoLab2, lab2_ids, 6u);
}
static void check_retained_interfaces(IEcoInterfaceBus1* bus) {
    IEcoUnknown* root; IEcoUnknown* identity; IEcoCalculatorX* x;
    IEcoCalculatorY* y; IEcoPolynomialMath* math;
    uint32_t live, refs; int16_t status; double coefficients[2];
    const double points[3] = {-1.0, 0.0, 1.0};
    live = real_memory.live; refs = probe_reference_count();
    root = 0; x = 0; y = 0; math = 0;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoLab2, 0, &IID_IEcoUnknown, (voidptr_t*)&root);
    check(status == ERR_ECO_SUCCESS && root != 0);
    if (root != 0) {
        check(root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorX, (voidptr_t*)&x) == ERR_ECO_SUCCESS && x != 0);
        check(root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorY, (voidptr_t*)&y) == ERR_ECO_SUCCESS && y != 0);
        check(root->pVTbl->QueryInterface(root, &IID_IEcoPolynomialMath, (voidptr_t*)&math) == ERR_ECO_SUCCESS && math != 0);
        check(root->pVTbl->Release(root) > 0u);
        if (x != 0) {
            check(x->pVTbl->Addition(x, 4.0, 5.0) == 9.0);
            identity = 0;
            check(x->pVTbl->QueryInterface(x, &IID_IEcoUnknown, (voidptr_t*)&identity) == ERR_ECO_SUCCESS && identity == root);
            if (identity != 0) identity->pVTbl->Release(identity);
            x->pVTbl->Release(x);
        }
        if (math != 0) {
            status = math->pVTbl->polyfit(math, points, points, 3u, 1u, coefficients);
            check(status == ERR_ECO_SUCCESS && close_to(coefficients[0], 1.0, 1e-12));
            math->pVTbl->Release(math);
        }
        if (y != 0) {
            check(y->pVTbl->Division(y, 9.0, 3.0) == 3.0);
            check(y->pVTbl->Release(y) == 0u);
        }
    }
    check(real_memory.live == live && probe_reference_count() == refs);
    ++lifetime_cases;
    root = 0; x = 0;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoCalculatorE, 0, &IID_IEcoUnknown, (voidptr_t*)&root);
    check(status == ERR_ECO_SUCCESS && root != 0);
    if (root != 0) {
        check(root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorX, (voidptr_t*)&x) == ERR_ECO_SUCCESS && x != 0);
        check(root->pVTbl->Release(root) > 0u);
        if (x != 0) {
            check(x->pVTbl->Subtraction(x, 8.0, 3.0) == 5.0);
            check(x->pVTbl->Release(x) == 0u);
        }
    }
    check(real_memory.live == live && probe_reference_count() == refs);
    ++lifetime_cases;
}
static void check_factories(IEcoUnknown* system, IEcoInterfaceBus1* bus) {
    IEcoComponentFactory* factories[8];
    IEcoUnknown* outer; IEcoUnknown* inner; IEcoUnknown* pub; IEcoUnknown* identity;
    uint32_t i, refs, live, before, after; int16_t status; voidptr_t value;
    factories[0] = GetCalculatorAFactory();
    factories[1] = GetCalculatorBFactory();
    factories[2] = GetCalculatorCFactory();
    factories[3] = GetCalculatorDFactory();
    factories[4] = GetCalculatorEFactory();
    factories[5] = GetPolynomialFactory_00000045();
    factories[6] = GetPolynomialQRFactory_00000046();
    factories[7] = GetEcoLab2Factory();
    outer = 0;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoLab2, 0, &IID_IEcoUnknown, (voidptr_t*)&outer);
    check(status == ERR_ECO_SUCCESS && outer != 0);
    if (outer == 0) return;
    for (i = 0u; i < 8u; ++i) {
        refs = probe_reference_count(); live = real_memory.live;
        value = (voidptr_t)1;
        check(factories[i]->pVTbl->Alloc(factories[i], 0, 0, &IID_IEcoUnknown, &value) == ERR_ECO_POINTER && value == 0);
        value = (voidptr_t)1;
        check(factories[i]->pVTbl->Alloc(0, system, 0, &IID_IEcoUnknown, &value) == ERR_ECO_POINTER && value == 0);
        check(factories[i]->pVTbl->Alloc(factories[i], system, 0, &IID_IEcoUnknown, 0) == ERR_ECO_POINTER);
        value = (voidptr_t)1;
        check(factories[i]->pVTbl->Alloc(factories[i], system, 0, 0, &value) == ERR_ECO_POINTER && value == 0);
        value = (voidptr_t)1;
        status = factories[i]->pVTbl->Alloc(factories[i], system, 0, &IID_IEcoPolynomial, &value);
        check(status == ERR_ECO_NOINTERFACE && value == 0);
        value = (voidptr_t)1;
        status = factories[i]->pVTbl->Alloc(factories[i], system, outer, &IID_IEcoPolynomialMath, &value);
        check(status == ERR_ECO_NOAGGREGATION && value == 0);
        inner = 0;
        status = factories[i]->pVTbl->Alloc(factories[i], system, outer, &IID_IEcoUnknown, (voidptr_t*)&inner);
        if (i == 7u) {
            check(status == ERR_ECO_NOAGGREGATION && inner == 0);
        } else {
            check(status == ERR_ECO_SUCCESS && inner != 0 && inner != outer);
            if (inner != 0) {
                pub = 0; identity = 0;
                status = inner->pVTbl->QueryInterface(inner,
                    &IID_IEcoCalculatorX, (voidptr_t*)&pub);
                check(status == ERR_ECO_SUCCESS && pub != 0);
                if (pub != 0) {
                    status = pub->pVTbl->QueryInterface(pub, &IID_IEcoUnknown, (voidptr_t*)&identity);
                    check(status == ERR_ECO_SUCCESS && identity == outer);
                    if (identity != 0) identity->pVTbl->Release(identity);
                    before = outer->pVTbl->AddRef(outer); outer->pVTbl->Release(outer);
                    pub->pVTbl->AddRef(pub); after = outer->pVTbl->AddRef(outer); outer->pVTbl->Release(outer);
                    check(after == before + 1u); pub->pVTbl->Release(pub); pub->pVTbl->Release(pub);
                }
                check(inner->pVTbl->Release(inner) == 0u);
            }
        }
        check(real_memory.live == live && probe_reference_count() == refs);
        ++factory_cases;
    }
    check(outer->pVTbl->Release(outer) == 0u);
}
static int16_t check_staged_registration(IEcoInterfaceBus1* bus) {
    const UGUID* cids[8]; IEcoComponentFactory* factories[8];
    uint32_t i, live, refs; IEcoUnknown* object; int16_t status;
    cids[0] = &CID_EcoLab2; factories[0] = GetEcoLab2Factory();
    cids[1] = &CID_EcoPolynomialMath; factories[1] = GetPolynomialFactory_00000045();
    cids[2] = &CID_EcoCalculatorC; factories[2] = GetCalculatorCFactory();
    cids[3] = &CID_EcoCalculatorA; factories[3] = GetCalculatorAFactory();
    cids[4] = &CID_EcoCalculatorD; factories[4] = GetCalculatorDFactory();
    cids[5] = &CID_EcoCalculatorB; factories[5] = GetCalculatorBFactory();
    cids[6] = &CID_EcoCalculatorE; factories[6] = GetCalculatorEFactory();
    cids[7] = &CID_EcoPolynomialMathQR; factories[7] = GetPolynomialQRFactory_00000046();
    for (i = 0u; i < 8u; ++i) {
        /* The vendor bus retains registry capacity until process shutdown.
         * Track component allocations, excluding only RegisterComponent itself. */
        real_memory.enabled = 0u;
        status = bus->pVTbl->RegisterComponent(bus, cids[i], (IEcoUnknown*)factories[i]);
        real_memory.enabled = 1u;
        check(status == ERR_ECO_SUCCESS);
        if (status != ERR_ECO_SUCCESS) return status;
        if (i < 6u) {
            live = real_memory.live; refs = probe_reference_count(); object = 0;
            status = bus->pVTbl->QueryComponent(bus, &CID_EcoLab2, 0, &IID_IEcoUnknown, (voidptr_t*)&object);
            check(status != ERR_ECO_SUCCESS && object == 0);
            if (object != 0) object->pVTbl->Release(object);
            check(real_memory.live == live && probe_reference_count() == refs);
            ++partial_cases;
        }
    }
    return ERR_ECO_SUCCESS;
}
static void check_failure_paths(IEcoInterfaceBus1* bus) {
    const UGUID* cids[8];
    IEcoUnknown* object; uint32_t i, fail, refs, live; int success; int16_t status;
    cids[0] = &CID_EcoCalculatorA; cids[1] = &CID_EcoCalculatorB;
    cids[2] = &CID_EcoCalculatorC; cids[3] = &CID_EcoCalculatorD;
    cids[4] = &CID_EcoCalculatorE; cids[5] = &CID_EcoPolynomialMath;
    cids[6] = &CID_EcoPolynomialMathQR; cids[7] = &CID_EcoLab2;
    for (i = 0u; i < 8u; ++i) {
        success = 0;
        for (fail = 1u; fail <= 64u; ++fail) {
            refs = probe_reference_count(); live = real_memory.live;
            real_memory.calls = 0u; real_memory.fail_at = fail; object = 0;
            status = bus->pVTbl->QueryComponent(bus, cids[i], 0, &IID_IEcoUnknown, (voidptr_t*)&object);
            real_memory.fail_at = 0u;
            check((status == ERR_ECO_SUCCESS) == (object != 0));
            if (object != 0) { check(object->pVTbl->Release(object) == 0u); success = 1; }
            check(real_memory.live == live && probe_reference_count() == refs);
            if (success) break;
        }
        check(success);
        ++allocation_cases;
    }

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

#define REPORT_ROW(name,values,count) ((void)0)
#include "MathRegression.h"
#undef REPORT_ROW
static void verify_component(IEcoPolynomialMathPtr component) {
    check(component != 0); if (component == 0) return;
    check_polynomial(component); check_constant_and_scale(component);
    check_overloads(component); check_services(component); check_errors(component);
    check_extended_math(component);
    ++math_cases;
    check(component->pVTbl->AddRef(component) > 1u);
    check(component->pVTbl->Release(component) > 0u);
}

int16_t EcoMain(IEcoUnknown* unknown) {
    IEcoSystem1* system; IEcoInterfaceBus1* bus; IEcoPolynomialMathPtr component;
    IEcoMemoryAllocator1* memory; IEcoMemoryAllocator1* again; uint32_t base_refs;
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

    memory = 0; again = 0;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoMemoryManager1, 0,
        &IID_IEcoMemoryAllocator1, (voidptr_t*)&memory);
    check(status == ERR_ECO_SUCCESS && memory != 0);
    if (memory == 0) { bus->pVTbl->Release(bus); system->pVTbl->Release(system); return ERR_ECO_POINTER; }
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoMemoryManager1, 0,
        &IID_IEcoMemoryAllocator1, (voidptr_t*)&again);
    check(status == ERR_ECO_SUCCESS && again == memory);
    if (again != 0) again->pVTbl->Release(again);
    probe_start(memory); base_refs = probe_reference_count();
    status = check_staged_registration(bus);
    if (status != ERR_ECO_SUCCESS) {
        probe_stop(); memory->pVTbl->Release(memory);
        bus->pVTbl->Release(bus); system->pVTbl->Release(system); return status;
    }
    base_refs = probe_reference_count();
    check_all_components(bus);
    check_retained_interfaces(bus);
    check_factories(unknown, bus);
    check_failure_paths(bus);
    component = 0;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoLab2, 0,
        &IID_IEcoPolynomialMath, (voidptr_t*)&component);
    check(status == ERR_ECO_SUCCESS && component != 0);
    if (component != 0) { verify_component(component); component->pVTbl->Release(component); }
    for (i = 0u; i < 2u; ++i) {
        component = 0;
        status = bus->pVTbl->QueryComponent(bus, ids[i], 0,
            &IID_IEcoPolynomialMath, (voidptr_t*)&component);
        check(status == ERR_ECO_SUCCESS && component != 0);
        if (component != 0) { verify_component(component); component->pVTbl->Release(component); }
    }
    /* Reverse insertion order avoids a known tombstone lookup bug in the
     * supplied Eco bus. No query is made after unregistration begins. */
    check(bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMathQR) == ERR_ECO_SUCCESS);
    check(bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorE) == ERR_ECO_SUCCESS);
    check(bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorB) == ERR_ECO_SUCCESS);
    check(bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorD) == ERR_ECO_SUCCESS);
    check(bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorA) == ERR_ECO_SUCCESS);
    check(bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorC) == ERR_ECO_SUCCESS);
    check(bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMath) == ERR_ECO_SUCCESS);
    check(bus->pVTbl->UnRegisterComponent(bus, &CID_EcoLab2) == ERR_ECO_SUCCESS);
    check(real_memory.live == 0u && real_memory.allocations > 0u && real_memory.allocations == real_memory.frees);
    check(real_memory.invalid == 0u && probe_reference_count() == base_refs);
    check(matrix_cases == 8u && factory_cases == 8u && allocation_cases == 8u &&
        partial_cases == 6u && math_cases == 3u && lifetime_cases == 2u);
    printf("Eco allocator: allocations=%lu frees=%lu live=%lu\n", (unsigned long)real_memory.allocations,
        (unsigned long)real_memory.frees, (unsigned long)real_memory.live);
    probe_stop(); memory->pVTbl->Release(memory);
    bus->pVTbl->Release(bus); system->pVTbl->Release(system);
    printf("Проверок: %lu, ошибок: %lu\n", (unsigned long)checks,
        (unsigned long)failures);
    return failures == 0u ? ERR_ECO_SUCCESS : ERR__POLYFIT_ARGUMENT;
}

