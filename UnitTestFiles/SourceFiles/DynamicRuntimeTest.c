/* Dynamic Eco integration client. No laboratory factories are linked here. */
#include <stdio.h>
#include <stdlib.h>
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IdEcoPolynomialMath.h"
#include "IdEcoLab2.h"
#include "Err.h"

static uint32_t checks;
static uint32_t failures;

static void check(int condition, const char* description) {
    ++checks;
    if (!condition) {
        ++failures;
        printf("FAIL: %s\n", description);
    }
}

static int close_to(double actual, double expected) {
    double delta = actual - expected;
    if (delta < 0.0) delta = -delta;
    return delta < 1.0e-8;
}

static void verify_math(IEcoInterfaceBus1* bus, const UGUID* cid) {
    const double x[5] = {-2.0, -1.0, 0.0, 1.0, 2.0};
    const double y[5] = {11.0, 5.0, 3.0, 5.0, 11.0};
    double coefficients[3];
    IEcoPolynomialMath* math;
    int16_t status;
    math = 0;
    status = bus->pVTbl->QueryComponent(bus, cid, 0,
        &IID_IEcoPolynomialMath, (voidptr_t*)&math);
    check(status == 0 && math != 0, "dynamic polyfit CID lookup");
    if (math == 0) return;
    status = math->pVTbl->polyfit(math, x, y, 5u, 2u, coefficients);
    check(status == 0, "dynamic polyfit call");
    check(close_to(coefficients[0], 2.0) && close_to(coefficients[1], 0.0) &&
        close_to(coefficients[2], 3.0), "dynamic polyfit coefficients");
    math->pVTbl->Release(math);
}

static void verify_calculator(IEcoInterfaceBus1* bus, const UGUID* cid,
    int supports_x, int supports_y) {
    IEcoUnknown* unknown;
    IEcoUnknown* from_x;
    IEcoUnknown* from_y;
    IEcoCalculatorX* x;
    IEcoCalculatorY* y;
    int16_t status;
    double value;
    unknown = 0; from_x = 0; from_y = 0; x = 0; y = 0;
    status = bus->pVTbl->QueryComponent(bus, cid, 0, &IID_IEcoUnknown,
        (voidptr_t*)&unknown);
    check(status == 0 && unknown != 0, "dynamic calculator CID lookup");
    if (unknown == 0) return;
    if (supports_x) {
        status = unknown->pVTbl->QueryInterface(unknown, &IID_IEcoCalculatorX,
            (voidptr_t*)&x);
        check(status == 0 && x != 0, "dynamic calculator X");
        if (x != 0) {
            check(x->pVTbl->Addition(x, -2.5, 6.0) == 3.5 &&
                x->pVTbl->Subtraction(x, 6.0, -2.5) == 8.5,
                "dynamic X operations");
            status = x->pVTbl->QueryInterface(x, &IID_IEcoUnknown,
                (voidptr_t*)&from_x);
            check(status == 0 && from_x == unknown, "dynamic X identity");
        }
    }
    if (supports_y) {
        status = unknown->pVTbl->QueryInterface(unknown, &IID_IEcoCalculatorY,
            (voidptr_t*)&y);
        check(status == 0 && y != 0, "dynamic calculator Y");
        if (y != 0) {
            check(y->pVTbl->Multiplication(y, -2.5, 6.0) == -15.0 &&
                y->pVTbl->Division(y, 7.0, 2.0) == 3.5,
                "dynamic Y operations");
            value = 1.0;
            status = y->pVTbl->TryDivision(y, 1.0, 0.0, &value);
            check(status != 0 && value == 0.0, "dynamic zero division status");
            status = y->pVTbl->QueryInterface(y, &IID_IEcoUnknown,
                (voidptr_t*)&from_y);
            check(status == 0 && from_y == unknown, "dynamic Y identity");
        }
    }
    if (from_y != 0) from_y->pVTbl->Release(from_y);
    if (from_x != 0) from_x->pVTbl->Release(from_x);
    if (y != 0) y->pVTbl->Release(y);
    if (x != 0) x->pVTbl->Release(x);
    unknown->pVTbl->Release(unknown);
}

static void verify_lab2(IEcoInterfaceBus1* bus) {
    IEcoLab2* lab2;
    IEcoLab1* lab1;
    IEcoLab2* roundtrip;
    IEcoUnknown* outer;
    IEcoUnknown* inner;
    int16_t status;
    lab2 = 0; lab1 = 0; roundtrip = 0; outer = 0; inner = 0;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoLab2, 0, &IID_IEcoLab2,
        (voidptr_t*)&lab2);
    check(status == 0 && lab2 != 0, "dynamic Lab2 CID lookup");
    if (lab2 == 0) return;
    status = lab2->pVTbl->QueryInterface(lab2, &IID_IEcoLab1, (voidptr_t*)&lab1);
    check(status == 0 && lab1 != 0, "dynamic Lab2 to Lab1");
    if (lab1 != 0) {
        status = lab1->pVTbl->QueryInterface(lab1, &IID_IEcoLab2,
            (voidptr_t*)&roundtrip);
        check(status == 0 && roundtrip == lab2, "dynamic aggregate QI roundtrip");
        status = lab1->pVTbl->QueryInterface(lab1, &IID_IEcoUnknown,
            (voidptr_t*)&inner);
        check(status == 0 && inner != 0, "dynamic aggregate inner identity");
    }
    status = lab2->pVTbl->QueryInterface(lab2, &IID_IEcoUnknown,
        (voidptr_t*)&outer);
    check(status == 0 && outer != 0 && outer == inner,
        "dynamic aggregate controlling unknown");
    if (roundtrip != 0) roundtrip->pVTbl->Release(roundtrip);
    if (inner != 0) inner->pVTbl->Release(inner);
    if (outer != 0) outer->pVTbl->Release(outer);
    if (lab1 != 0) lab1->pVTbl->Release(lab1);
    lab2->pVTbl->Release(lab2);
}

int16_t EcoMain(IEcoUnknown* unknown) {
    IEcoSystem1* system;
    IEcoInterfaceBus1* bus;
    char* runtime;
    int16_t status;
    system = 0; bus = 0;
    runtime = getenv("ECO_LAB_RUNTIME");
    if (unknown == 0 || runtime == 0 || *runtime == '\0') return -1;
    status = unknown->pVTbl->QueryInterface(unknown, &GID_IEcoSystem,
        (voidptr_t*)&system);
    if (status != 0 || system == 0) return -1;
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&bus);
    if (status != 0 || bus == 0) {
        system->pVTbl->Release(system); return -1;
    }
    /* ECO_FRAMEWORK_RT is consumed by the official macOS bus loader. */
    check(getenv("ECO_FRAMEWORK_RT") != 0, "official runtime path configured");
    printf("Official Eco loader runtime: %s\n", runtime);
    /* No RegisterComponent: discovery resolves each CID to its dylib. */
    verify_math(bus, &CID_EcoPolynomialMath);
    verify_math(bus, &CID_EcoPolynomialMathQR);
    verify_calculator(bus, &CID_EcoCalculatorA, 1, 0);
    verify_calculator(bus, &CID_EcoCalculatorB, 1, 0);
    verify_calculator(bus, &CID_EcoCalculatorC, 1, 1);
    verify_calculator(bus, &CID_EcoCalculatorD, 1, 1);
    verify_calculator(bus, &CID_EcoCalculatorE, 1, 1);
    verify_lab2(bus);
    bus->pVTbl->Release(bus);
    system->pVTbl->Release(system);
    if (checks != 44u) {
        ++failures;
        printf("FAIL: expected 44 runtime checks, executed %lu\n",
            (unsigned long)checks);
    }
    printf("Dynamic runtime: checks=%lu failures=%lu\n",
        (unsigned long)checks, (unsigned long)failures);
    return failures == 0u ? 0 : -1;
}
