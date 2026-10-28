/* Public-only client. X/Y and calculator CID headers come from ExternalContracts. */
#include <stdio.h>
#include <stdlib.h>
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IdEcoCalculatorA.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorC.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoCalculatorSafe.h"
#include "IdEcoPolynomialMath.h"
#include "IdEcoLab2.h"

static unsigned checks, failures, components;
#define CHECK(condition, message) do { ++checks; if (!(condition)) { ++failures; printf("FAIL: %s (line %d)\n", message, __LINE__); } } while (0)
static int near(double a, double b) { double d = a - b; return d < 1e-8 && d > -1e-8; }

static void inspect(IEcoInterfaceBus1* bus, const UGUID* cid, const char* name,
    int has_y, int has_math, int has_lab2) {
    IEcoUnknown* root; IEcoUnknown* interfaces[7]; IEcoUnknown* other; IEcoUnknown* identity;
    const UGUID* iids[7]; IEcoCalculatorX* x; IEcoCalculatorY* y;
    IEcoCalculatorSafe* safe; IEcoPolynomialMath* math;
    unsigned count, i, j; uint32_t baseline; int16_t status, value;
    double coeffs[3]; const double points[5] = {-2, -1, 0, 1, 2};
    const double values[5] = {11, 5, 3, 5, 11};
    root = 0;
    status = bus->pVTbl->QueryComponent(bus, cid, 0, &IID_IEcoUnknown, (voidptr_t*)&root);
    CHECK(status == 0 && root != 0, "QueryComponent returns a real component");
    if (root == 0) return;
    ++components;
    printf("%s: ", name);
    x = 0;
    status = root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorX, (voidptr_t*)&x);
    CHECK(status == 0 && x != 0, "official X IID");
    if (x != 0) {
        CHECK(x->pVTbl->Addition(x, 3, 5) == 8, "Addition");
        CHECK(x->pVTbl->Subtraction(x, 3, 5) == -2, "Subtraction");
        CHECK(x->pVTbl->Addition(x, 32767, 32767) == 65534L, "int32 addition result");
        x->pVTbl->Release(x);
    }
    y = 0;
    status = root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorY, (voidptr_t*)&y);
    CHECK(has_y ? status == 0 && y != 0 : status == ERR_ECO_NOINTERFACE && y == 0, "A/C interface set");
    if (y != 0) {
        CHECK(y->pVTbl->Multiplication(y, 6, 7) == 42, "Multiplication");
        CHECK(y->pVTbl->Division(y, 20, 3) == 6, "integer Division");
        CHECK(y->pVTbl->Division(y, -20, 3) == -6, "negative Division");
        y->pVTbl->Release(y);
    }
    if (has_y) {
        safe = 0;
        status = root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorSafe, (voidptr_t*)&safe);
        CHECK(status == 0 && safe != 0, "separate checked interface");
        if (safe != 0) {
            value = 99;
            CHECK(safe->pVTbl->TryDivision(safe, 1, 0, &value) == ERR_ECO_CALCULATOR_ZERO && value == 0, "checked division by zero");
            CHECK(safe->pVTbl->TryDivision(safe, -32768, -1, &value) == ERR_ECO_CALCULATOR_OVERFLOW && value == 0, "checked division overflow");
            safe->pVTbl->Release(safe);
        }
    }
    if (has_math) {
        math = 0;
        status = root->pVTbl->QueryInterface(root, &IID_IEcoPolynomialMath, (voidptr_t*)&math);
        CHECK(status == 0 && math != 0, "polyfit exposed through aggregate");
        if (math != 0) {
            status = math->pVTbl->polyfit(math, points, values, 5u, 2u, coeffs);
            CHECK(status == 0 && near(coeffs[0], 2) && near(coeffs[1], 0) && near(coeffs[2], 3), "real polyfit result");
            math->pVTbl->Release(math);
        }
    }
    count = 0u; iids[count++] = &IID_IEcoUnknown; iids[count++] = &IID_IEcoCalculatorX;
    if (has_y) { iids[count++] = &IID_IEcoCalculatorY; iids[count++] = &IID_IEcoCalculatorSafe; }
    if (has_math) { iids[count++] = &IID_IEcoLab1; iids[count++] = &IID_IEcoPolynomialMath; }
    if (has_lab2) iids[count++] = &IID_IEcoLab2;
    for (i = 0u; i < count; ++i) {
        interfaces[i] = 0;
        status = root->pVTbl->QueryInterface(root, iids[i], (voidptr_t*)&interfaces[i]);
        CHECK(status == 0 && interfaces[i] != 0, "initial interface in matrix");
        if (interfaces[i] == 0) continue;
        for (j = 0u; j < count; ++j) {
            other = 0; identity = 0;
            status = interfaces[i]->pVTbl->QueryInterface(interfaces[i], iids[j], (voidptr_t*)&other);
            CHECK(status == 0 && other != 0, "pairwise QueryInterface");
            if (other != 0) {
                status = other->pVTbl->QueryInterface(other, &IID_IEcoUnknown, (voidptr_t*)&identity);
                CHECK(status == 0 && identity == root, "canonical controlling unknown");
                if (identity != 0) identity->pVTbl->Release(identity);
                other->pVTbl->Release(other);
            }
        }
        other = (IEcoUnknown*)1;
        CHECK(interfaces[i]->pVTbl->QueryInterface(interfaces[i], &IID_IEcoPolynomial, (voidptr_t*)&other) == ERR_ECO_NOINTERFACE && other == 0, "unknown IID clears output");
        baseline = root->pVTbl->AddRef(root); root->pVTbl->Release(root);
        CHECK(interfaces[i]->pVTbl->AddRef(interfaces[i]) == baseline, "shared public reference counter");
        interfaces[i]->pVTbl->Release(interfaces[i]);
        interfaces[i]->pVTbl->Release(interfaces[i]);
    }
    CHECK(root->pVTbl->Release(root) == 0u, "last release destroys component");
    printf("official X%s, QI matrix and identity checked\n", has_y ? "/Y" : "");
}

static void horner_and_lifetime(IEcoInterfaceBus1* bus) {
    IEcoLab2* lab2; IEcoLab2* roundtrip; IEcoCalculatorX* x;
    IEcoUnknown* identity; IEcoUnknown* root;
    int16_t status, point; int32_t value, expected; unsigned i;
    const int16_t coeffs[4] = {2, -3, 0, 5};
    const int16_t overflow[3] = {100, 0, 0};
    const int16_t final_wide[2] = {32767, 32767};
    lab2 = 0; x = 0; root = 0;
    status = bus->pVTbl->QueryComponent(bus, &CID_EcoLab2, 0, &IID_IEcoLab2, (voidptr_t*)&lab2);
    CHECK(status == 0 && lab2 != 0, "Lab2 root");
    if (lab2 == 0) return;
    for (point = -3; point <= 4; ++point) {
        expected = 0;
        for (i = 0; i < 4; ++i) expected = expected * point + coeffs[i];
        status = lab2->pVTbl->Horner(lab2, coeffs, 4u, point, &value);
        CHECK(status == 0 && value == expected, "Horner via included A/C and D");
    }
    value = 99;
    CHECK(lab2->pVTbl->Horner(lab2, overflow, 3u, 100, &value) == ERR_ECO_CALCULATOR_OVERFLOW && value == 0, "Horner checks int16 intermediates");
    CHECK(lab2->pVTbl->Horner(lab2, final_wide, 2u, 1, &value) == 0 && value == 65534L, "Horner final int32 result");
    CHECK(lab2->pVTbl->Horner(lab2, coeffs, 0u, 1, &value) == ERR_ECO_INVALIDARG && value == 0, "empty polynomial");
    CHECK(lab2->pVTbl->Horner(lab2, 0, 4u, 1, &value) == ERR_ECO_POINTER && value == 0, "NULL coefficients");
    CHECK(lab2->pVTbl->Horner(0, coeffs, 4u, 1, &value) == ERR_ECO_POINTER && value == 0, "NULL receiver");
    CHECK(lab2->pVTbl->Horner(lab2, coeffs, 4u, 1, 0) == ERR_ECO_POINTER, "NULL result");
    CHECK(lab2->pVTbl->Horner(lab2, coeffs, 0xFFFFFFFFu, 1, &value) == ERR_ECO_INVALIDARG && value == 0, "size overflow");
    CHECK(lab2->pVTbl->QueryInterface(lab2, &IID_IEcoUnknown, (voidptr_t*)&root) == 0 && root != 0, "outer identity");
    CHECK(lab2->pVTbl->QueryInterface(lab2, &IID_IEcoCalculatorX, (voidptr_t*)&x) == 0 && x != 0, "B.X through Lab2/Lab1");
    if (root != 0) root->pVTbl->Release(root);
    lab2->pVTbl->Release(lab2);
    if (x != 0) {
        CHECK(x->pVTbl->Addition(x, 4, 5) == 9, "B.X survives release of original Lab2 reference");
        identity = 0; roundtrip = 0;
        CHECK(x->pVTbl->QueryInterface(x, &IID_IEcoUnknown, (voidptr_t*)&identity) == 0 && identity == root, "B still controlled by Lab2");
        CHECK(x->pVTbl->QueryInterface(x, &IID_IEcoLab2, (voidptr_t*)&roundtrip) == 0 && roundtrip == lab2, "B.X -> outer Lab2");
        if (identity != 0) identity->pVTbl->Release(identity);
        if (roundtrip != 0) roundtrip->pVTbl->Release(roundtrip);
        CHECK(x->pVTbl->Release(x) == 0u, "B.X final release destroys entire aggregate");
    }
    printf("Lab2: Horner through included A/C+D; Lab2 -> Lab1 -> B lifetime checked\n");
}

int16_t EcoMain(IEcoUnknown* entry) {
    IEcoSystem1* system; IEcoInterfaceBus1* bus; int16_t status;
    const UGUID* ids[7]; const char* names[7]; unsigned i; int use_c;
    char* variant;
#ifndef ECO_DYNAMIC_CLIENT
    IEcoComponentFactory* factories[7];
#endif
    system = 0; bus = 0;
    variant = getenv("ECO_EXPECT_CALCULATOR");
    if (variant == 0 || (variant[0] != 'A' && variant[0] != 'C') || variant[1] != '\0') {
        printf("Set ECO_EXPECT_CALCULATOR=A or C\n"); return -1;
    }
    use_c = variant[0] == 'C';
    if (entry == 0) return -1;
    status = entry->pVTbl->QueryInterface(entry, &GID_IEcoSystem, (voidptr_t*)&system);
    if (status != 0 || system == 0) return -1;
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1, (voidptr_t*)&bus);
    if (status != 0 || bus == 0) { system->pVTbl->Release(system); return -1; }
    ids[0] = &CID_EcoCalculatorA; ids[1] = &CID_EcoCalculatorB;
    ids[2] = &CID_EcoCalculatorD; ids[3] = &CID_EcoCalculatorE;
    ids[4] = &CID_EcoPolynomialMath; ids[5] = &CID_EcoPolynomialMathQR; ids[6] = &CID_EcoLab2;
    names[0] = use_c ? "C replacing A (same CID)" : "A"; names[1] = "B";
    names[2] = "D includes B"; names[3] = "E aggregates B";
    names[4] = "Lab1"; names[5] = "Lab1 QR"; names[6] = "Lab2";
#ifndef ECO_DYNAMIC_CLIENT
    factories[0] = GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E;
    factories[1] = GetIEcoComponentFactoryPtr_AE202E543CE54550899603BD70C62565;
    factories[2] = GetIEcoComponentFactoryPtr_3A8E44677E82475CB4A3719ED8397E61;
    factories[3] = GetIEcoComponentFactoryPtr_872FEF1DE3314B87AD44D1E7C232C2F0;
    factories[4] = GetPolynomialFactory_00000045(); factories[5] = GetPolynomialQRFactory_00000046();
    factories[6] = GetEcoLab2Factory();
    for (i = 0; i < 7; ++i) {
        status = bus->pVTbl->RegisterComponent(bus, ids[i], (IEcoUnknown*)factories[i]);
        if (status != 0) { bus->pVTbl->Release(bus); system->pVTbl->Release(system); return -1; }
    }
#endif
    for (i = 0; i < 7; ++i) inspect(bus, ids[i], names[i], i == 0 ? use_c : i != 1, i >= 4, i == 6);
    horner_and_lifetime(bus);
    CHECK(components == 7u, "all seven CID scenarios executed");
    bus->pVTbl->Release(bus); system->pVTbl->Release(system);
    printf("Public client %s: checks=%u failures=%u\n", variant, checks, failures);
    return failures == 0 ? 0 : -1;
}
