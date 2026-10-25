/* Shared executable numerical regressions; this file contains tests only. */
#include <limits.h>
#include <float.h>
static void check_integer_precision(IEcoPolynomialMath* component) {
    const long maximum[1] = {LONG_MAX};
    const long minimum[1] = {LONG_MIN};
    const long zero[1] = {0L};
    const long seven[1] = {7L};
    long double result[2]; int16_t status; int loss;
#if LONG_MAX > 2147483647L
    const long boundary_x[3] = {9007199254740992L, 9007199254740993L, 9007199254740994L};
    const long boundary_y[3] = {0L, 1L, 2L};
    loss = LDBL_MANT_DIG < 54;
    result[0] = 123.0L; result[1] = 456.0L;
    status = component->pVTbl->polyfit_long(component, boundary_x, boundary_y, 3u, 1u, result);
    check(status == (loss ? ERR__POLYFIT_PRECISION : ERR_ECO_SUCCESS));
    check(loss ? (result[0] == 0.0L && result[1] == 0.0L) :
        (close_long_double(result[0], 1.0L, 1.0e-12L) &&
         close_long_double(result[1], -9007199254740992.0L, 1.0L)));
#endif
    loss = LDBL_MANT_DIG < (int)(sizeof(long) * CHAR_BIT - 1u);
    result[0] = 123.0L;
    status = component->pVTbl->polyfit_long(component, maximum, seven, 1u, 0u, result);
    check(status == (loss ? ERR__POLYFIT_PRECISION : ERR_ECO_SUCCESS));
    check(result[0] == (loss ? 0.0L : 7.0L));
    result[0] = 123.0L;
    status = component->pVTbl->polyfit_long(component, zero, maximum, 1u, 0u, result);
    check(status == (loss ? ERR__POLYFIT_PRECISION : ERR_ECO_SUCCESS));
    check(result[0] == (loss ? 0.0L : (long double)LONG_MAX));
    result[0] = 123.0L;
    status = component->pVTbl->polyfit_long(component, minimum, seven, 1u, 0u, result);
    check(status == ERR_ECO_SUCCESS);
    check(result[0] == 7.0L);
}
static void check_extended_math(IEcoPolynomialMath* component) {
    const double tiny[3] = {-1.0e-310, 0.0, 1.0e-310};
    const double huge_x[3] = {1.0e308, 1.0e308, 1.0e308};
    const double seven[3] = {7.0, 7.0, 7.0};
    const double x[5] = {-2.0, -1.0, 0.0, 1.0, 2.0};
    const double noisy[5] = {-0.9, 0.9, 3.0, 5.1, 6.9};
    const double huge_y[5] = {1.0e308, 1.0e308, 1.0e308, 1.0e308, 1.0e308};
    const double opposite[3] = {-1.0e308, 0.0, 1.0e308};
    const double repeated[3] = {1.0, 1.0, 1.0};
    double bad[5]; double out[3]; volatile double zero;
    EcoVectorView vx; EcoVectorView vy; IEcoPolynomial* polynomial;
    IEcoUnknown* unknown; int16_t status; uint32_t i;
    check_integer_precision(component);
    status = component->pVTbl->polyfit(component, tiny, tiny, 3u, 1u, out);
    check(status == ERR_ECO_SUCCESS);
    check(close_to(out[0], 1.0, 1.0e-12) && close_to(out[1], 0.0, 1.0e-320));
    REPORT_ROW("tiny_identity", out, 2u);
    status = component->pVTbl->polyfit(component, huge_x, seven, 3u, 0u, out);
    check(status == ERR_ECO_SUCCESS && close_to(out[0], 7.0, 1.0e-12));
    REPORT_ROW("huge_x_constant", out, 1u);
    status = component->pVTbl->polyfit(component, x, huge_y, 5u, 0u, out);
    check(status == ERR_ECO_SUCCESS && close_to(out[0] / 1.0e308, 1.0, 1.0e-12));
    REPORT_ROW("huge_y_constant", out, 1u);
    status = component->pVTbl->polyfit(component, x, noisy, 5u, 1u, out);
    check(status == ERR_ECO_SUCCESS);
    check(close_to(out[0], 1.98, 1.0e-12) && close_to(out[1], 3.0, 1.0e-12));
    REPORT_ROW("noisy_linear", out, 2u);
    status = component->pVTbl->polyfit(component, opposite, opposite, 3u, 1u, out);
    check(status == ERR_ECO_SUCCESS && close_to(out[0], 1.0, 1.0e-12) && out[1] == 0.0);
    REPORT_ROW("opposite_extremes", out, 2u);
    status = component->pVTbl->polyfit(component, repeated, seven, 3u, 1u, out);
    check(status == ERR__POLYFIT_SINGULAR);
    check(out[0] == 0.0 && out[1] == 0.0);
    zero = 0.0;
    for (i = 0u; i < 5u; ++i) bad[i] = x[i];
    bad[2] = zero / zero;
    status = component->pVTbl->polyfit(component, bad, noisy, 5u, 1u, out);
    check(status == ERR__POLYFIT_NONFINITE);
    status = component->pVTbl->polyfit(component, x, bad, 5u, 1u, out);
    check(status == ERR__POLYFIT_NONFINITE);
    status = component->pVTbl->polyfit(component, x, 0, 5u, 1u, out);
    check(status == ERR_ECO_POINTER);
    status = component->pVTbl->polyfit(0, x, noisy, 5u, 1u, out);
    check(status == ERR_ECO_POINTER);
    status = component->pVTbl->polyfit(component, x, noisy, 0u, 1u, out);
    check(status == ERR__POLYFIT_ARGUMENT);
    status = component->pVTbl->polyfit(component, x, noisy, ECO_POLYFIT_MAX_COUNT + 1u, 1u, out);
    check(status == ERR__POLYFIT_ARGUMENT);
    status = component->pVTbl->polyfit(component, x, noisy, 5u, 0xFFFFFFFFu, out);
    check(status == ERR__POLYFIT_ARGUMENT);
    vx.data = x; vx.length = 5u; vy.data = noisy; vy.length = 4u;
    check(component->pVTbl->FitVector(component, &vx, &vy, 1u, out) == ERR__POLYFIT_ARGUMENT);
    check(component->pVTbl->FitVector(component, 0, &vy, 1u, out) == ERR_ECO_POINTER);
    check(component->pVTbl->Add(component, 1.0, 2.0, 0) == ERR_ECO_POINTER);
    check(component->pVTbl->Subtract(0, 1.0, 2.0, out) == ERR_ECO_POINTER);
    unknown = (IEcoUnknown*)component;
    check(unknown->pVTbl->QueryInterface(unknown, 0, (voidptr_t*)&polynomial) == ERR_ECO_POINTER);
    check(polynomial == 0);
    check(unknown->pVTbl->QueryInterface(unknown, &IID_IEcoUnknown, 0) == ERR_ECO_POINTER);
    check(unknown->pVTbl->QueryInterface(0, &IID_IEcoUnknown, (voidptr_t*)&polynomial) == ERR_ECO_POINTER);
    polynomial = 0;
    check(component->pVTbl->new_Polynomial(component, x, noisy, 5u, 1u, 0) == ERR_ECO_POINTER);
    status = component->pVTbl->new_Polynomial(component, x, noisy, 5u, 1u, &polynomial);
    check(status == ERR_ECO_SUCCESS && polynomial != 0);
    if (polynomial != 0) {
        check(polynomial->pVTbl->get_Coefficients(polynomial, 1u, out) == ERR__POLYFIT_BUFFER);
        check(polynomial->pVTbl->get_Coefficients(polynomial, 2u, 0) == ERR_ECO_POINTER);
        check(polynomial->pVTbl->get_Degree(polynomial, 0) == ERR_ECO_POINTER);
        check(polynomial->pVTbl->Evaluate(polynomial, 2.0, 0) == ERR_ECO_POINTER);
        check(polynomial->pVTbl->Release(polynomial) == 0u);
    }
}
