/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#include "polyfit_core.h"
#define MAX_ORDER 17u
#define EPSILON 1.0e-15L
#define MATRIX_ELEMENTS (MAX_ORDER * (MAX_ORDER + 1u))
#define WORK_ELEMENTS (MATRIX_ELEMENTS + 3u * (MAX_ORDER + 1u))
typedef long double (*ReadValue)(void*, uint32_t, uint32_t);

/* Return an absolute value without a dependency on math.h. */
static long double magnitude(long double x) { return x < 0 ? -x : x; }

/* Compute a square root with Newton iterations without math.h. */
static long double root_value(long double x) {
    long double estimate;
    long double next;
    uint32_t i;
    if (x == 0) return 0;
    estimate = x > 1 ? x : 1;
    for (i = 0; i < 256u; ++i) {
        next = (estimate + x / estimate) / 2;
        if (next == estimate) break;
        estimate = next;
    }
    return estimate;
}

/* Binary scaling avoids constructing 1/x_scale or x_scale^degree.
 * The final coefficient can be representable even when those intermediates
 * overflow (for example y=x with subnormal x). No runtime math API is used. */
static long double binary_fraction(long double value, int32_t* exponent) {
    *exponent = 0;
    while (value >= 1.0L) { value *= 0.5L; ++*exponent; }
    while (value < 0.5L) { value *= 2.0L; --*exponent; }
    return value;
}

static int16_t restore_coefficient(long double coefficient,
    long double y_scale, long double x_scale, uint32_t power,
    long double* out) {
    long double value, xm, ym, next;
    int32_t exponent, xe, ye;
    uint32_t i;
    int negative;
    *out = 0.0L;
    if (coefficient == 0.0L) return ERR_ECO_SUCCESS;
    if (coefficient - coefficient != 0.0L) return ERR__POLYFIT_NONFINITE;
    negative = coefficient < 0.0L;
    value = binary_fraction(magnitude(coefficient), &exponent);
    xm = binary_fraction(x_scale, &xe);
    ym = binary_fraction(y_scale, &ye);
    value *= ym; exponent += ye;
    for (i = 0u; i < power; ++i) {
        value /= xm; exponent -= xe;
        while (value >= 1.0L) { value *= 0.5L; ++exponent; }
        while (value < 0.5L) { value *= 2.0L; --exponent; }
    }
    while (exponent > 0) {
        next = value * 2.0L;
        if (next - next != 0.0L) return ERR__POLYFIT_NONFINITE;
        value = next; --exponent;
    }
    while (exponent < 0 && value != 0.0L) { value *= 0.5L; ++exponent; }
    *out = negative ? -value : value;
    return ERR_ECO_SUCCESS;
}

/* Solve one fit in O(n * d^2 + d^3) time and O(d^2) allocator memory. */
static int16_t solve(IEcoMemoryAllocator1* memory, ReadValue read,
    void* context, uint32_t count, uint32_t degree, long double* out,
    uint32_t method) {
    long double* block;
    long double (*a)[MAX_ORDER + 1u];
    long double* row;
    long double* result;
    long double* converted;
    long double scale;
    long double x_scale;
    long double y_scale;
    long double y_origin, y_min, y_max, y_span;
    long double center;
    long double x_min, x_max, span, origin;
    int raw_centering;
    long double a_scale;
    long double b_shift;
    long double xv, yv, z, m, av, bv, c, s, h, old, t, f, v;
    uint32_t n, i, j, k, p, order, bytes;
    int16_t status;
    if (memory == 0 || read == 0 || context == 0 || out == 0)
        return ERR_ECO_POINTER;
    if (degree >= MAX_ORDER || count < degree + 1u || count > ECO_POLYFIT_MAX_COUNT || method > 1u)
        return ERR__POLYFIT_ARGUMENT;
    /* Check before conversion to the uint32_t allocator API. */
    if (sizeof(long double) > 0xFFFFFFFFu / WORK_ELEMENTS)
        return ERR_ECO_OUTOFMEMORY;
    bytes = (uint32_t)(sizeof(long double) * WORK_ELEMENTS);
    block = (long double*)memory->pVTbl->Alloc(memory, bytes);
    if (block == 0) return ERR_ECO_OUTOFMEMORY;
    memory->pVTbl->Fill(memory, block, 0, bytes);
    a = (long double (*)[MAX_ORDER + 1u])block;
    row = block + MATRIX_ELEMENTS;
    result = row + MAX_ORDER + 1u;
    converted = result + MAX_ORDER + 1u;
    order = degree + 1u;
    scale = 0;
    center = 0;
    x_scale = 0;
    y_scale = 0;
    x_min = 0; x_max = 0; origin = 0;
    y_min = 0; y_max = 0; y_origin = 0;
    raw_centering = 0;
    status = ERR_ECO_SUCCESS;
    for (n = 0; n < count; ++n) {
        xv = read(context, n, 0); yv = read(context, n, 1);
        if (xv - xv != 0 || yv - yv != 0) {
            status = ERR__POLYFIT_NONFINITE; goto cleanup;
        }
        if (n == 0u || xv < x_min) x_min = xv;
        if (n == 0u || xv > x_max) x_max = xv;
        if (n == 0u || yv < y_min) y_min = yv;
        if (n == 0u || yv > y_max) y_max = yv;
        if (magnitude(xv) > x_scale) x_scale = magnitude(xv);
        if (magnitude(yv) > y_scale) y_scale = magnitude(yv);
    }
    if (x_scale == 0.0L) x_scale = 1.0L;
    if (y_scale == 0.0L) y_scale = 1.0L;
    y_span = y_max - y_min;
    if (y_span - y_span == 0.0L) {
        y_origin = y_min + y_span * 0.5L;
        y_scale = y_span > 0.0L ? y_span * 0.5L : 1.0L;
        if (y_scale == 0.0L) y_scale = y_span;
    }
    /* Use an exactly representable local difference where possible. Dividing
     * two close, large x values before subtraction would erase precision. */
    span = x_max - x_min;
    if (degree == 0u) {
        raw_centering = 1; x_scale = 1.0L; origin = 0.0L;
        a_scale = 1.0L; b_shift = 0.0L;
    }
    else if (span - span == 0.0L) {
        raw_centering = 1;
        origin = x_min + span * 0.5L;
        if (span > 0.0L) {
            x_scale = span * 0.5L;
            if (x_scale == 0.0L) x_scale = span;
        }
        a_scale = 1.0L; b_shift = -origin / x_scale;
    }
    else {
        /* Opposite extreme signs can overflow max-min. The normalized
         * endpoints remain bounded and their midpoint is safe. */
        center = (x_min / x_scale + x_max / x_scale) * 0.5L;
        scale = (x_max / x_scale - x_min / x_scale) * 0.5L;
        if (scale == 0.0L) scale = 1.0L;
        a_scale = 1.0L / scale; b_shift = -center / scale;
    }
    for (n = 0; n < count; ++n) {
        if (degree == 0u) z = 0.0L;
        else if (raw_centering) z = (read(context, n, 0) - origin) / x_scale;
        else z = (read(context, n, 0) / x_scale - center) / scale;
        row[order] = (read(context, n, 1) - y_origin) / y_scale;
        row[degree] = 1;
        for (i = degree; i > 0; --i) row[i - 1u] = row[i] * z;
        if (method == 0u) {
            for (i = 0; i < order; ++i)
                for (j = 0; j <= order; ++j) a[i][j] += row[i] * row[j];
        }
        else {
            for (i = 0; i < order; ++i) {
                av = a[i][i]; bv = row[i];
                m = magnitude(av) > magnitude(bv) ? magnitude(av) : magnitude(bv);
                if (m == 0) continue;
                h = m * root_value((av / m) * (av / m) + (bv / m) * (bv / m));
                c = av / h; s = bv / h;
                for (j = i; j <= order; ++j) {
                    old = a[i][j];
                    a[i][j] = c * old + s * row[j];
                    row[j] = -s * old + c * row[j];
                }
            }
        }
    }
    if (method == 0u) {
        for (i = 0; i < order; ++i) {
            p = i;
            for (j = i + 1u; j < order; ++j)
                if (magnitude(a[j][i]) > magnitude(a[p][i])) p = j;
            if (p != i) {
                for (k = i; k <= order; ++k) {
                    t = a[i][k]; a[i][k] = a[p][k]; a[p][k] = t;
                }
            }
            if (magnitude(a[i][i]) < EPSILON) {
                status = ERR__POLYFIT_SINGULAR; goto cleanup;
            }
            for (j = i + 1u; j < order; ++j) {
                f = a[j][i] / a[i][i];
                for (k = i; k <= order; ++k) a[j][k] -= f * a[i][k];
            }
        }
    }
    for (i = order; i > 0u; ) {
        --i;
        v = a[i][order];
        if (magnitude(a[i][i]) < EPSILON) {
            status = ERR__POLYFIT_SINGULAR; goto cleanup;
        }
        for (j = i + 1u; j < order; ++j) v -= a[i][j] * result[j];
        result[i] = v / a[i][i];
    }
    /* Compose p((x-center)/scale) in the original x basis. */
    /* Rebuild with a small fixed temporary using Horner composition. */
    /* Keep temporary initialization inside the Eco allocator contract. */
    memory->pVTbl->Fill(memory, converted, 0,
        (uint32_t)(sizeof(long double) * order));
    for (i = 0; i < order; ++i) {
        uint32_t old_degree;
        uint32_t j;
        old_degree = i;
        if (i != 0u) {
            for (j = old_degree; j > 0u; --j)
                converted[j] = converted[j - 1u] * b_shift + converted[j] * a_scale;
            converted[0] = converted[0] * a_scale;
        }
        converted[old_degree] += result[i];
    }
    /* Restore the removed y offset only in the constant coefficient. */
    converted[degree] += y_origin / y_scale;
    for (i = 0; i < order; ++i) {
        status = restore_coefficient(converted[i], y_scale, x_scale,
            degree - i, &result[i]);
        if (status != ERR_ECO_SUCCESS) goto cleanup;
    }
    memory->pVTbl->Copy(memory, out, result, (uint32_t)(sizeof(long double) * order));
cleanup:
    memory->pVTbl->Free(memory, block);
    return status;
}

/* Generate a typed view over x/y input arrays. */
#define PAIR(T,N) \
    typedef struct N { const T* x; const T* y; } N; \
    static long double N##_read(void* context, uint32_t i, uint32_t column) { \
        N* pair = (N*)context; \
        return column == 0u ? (long double)pair->x[i] : (long double)pair->y[i]; \
    }
PAIR(double, DoublePair)
PAIR(float, FloatPair)
PAIR(long double, LongDoublePair)
PAIR(int, IntPair)
PAIR(long, LongPair)

/* Integer inputs must reach the floating solver without silently losing
 * low bits. Split before conversion; both the 16-bit remainder and scaled
 * quotient are exact even when long double has only a double mantissa.
 * No floating-to-integer cast (undefined at rounded LONG_MAX) is needed. */
static int integer_input_exact(long value) {
    long double high;
    high = (long double)(value / 65536L) * 65536.0L;
    return (long double)value - high == (long double)(value % 65536L);
}
static int floating_input_exact(long double value) {
    (void)value;
    return 1;
}

/* Generate one typed entry point; conversion is arithmetic and the final
 * same-type transfer uses IEcoMemoryAllocator1::Copy. */
#define API(S,T,O,N,LIMIT,VALIDATE) \
int16_t S##_00000045(/* in */ IEcoMemoryAllocator1* memory, \
    /* in */ const T* x, /* in */ const T* y, /* in */ uint32_t count, \
    /* in */ uint32_t degree, /* out */ O* out, /* in */ uint32_t method) { \
    N pair; long double* result; O* converted; \
    uint32_t i, bytes; int16_t status; \
    if (memory == 0 || out == 0) return ERR_ECO_POINTER; \
    if (degree >= MAX_ORDER) return ERR__POLYFIT_ARGUMENT; \
    if (sizeof(O) > 0xFFFFFFFFu / (degree + 1u)) return ERR_ECO_OUTOFMEMORY; \
    bytes = (uint32_t)(sizeof(O) * (degree + 1u)); \
    memory->pVTbl->Fill(memory, out, 0, bytes); \
    if (x == 0 || y == 0) return ERR_ECO_POINTER; \
    if (count < degree + 1u || count > ECO_POLYFIT_MAX_COUNT || method > 1u) return ERR__POLYFIT_ARGUMENT; \
    for (i = 0u; i < count; ++i) \
        if (!VALIDATE(x[i]) || !VALIDATE(y[i])) return ERR__POLYFIT_PRECISION; \
    if (sizeof(long double) > 0xFFFFFFFFu / (2u * MAX_ORDER)) return ERR_ECO_OUTOFMEMORY; \
    result = (long double*)memory->pVTbl->Alloc(memory, (uint32_t)(sizeof(long double) * 2u * MAX_ORDER)); \
    if (result == 0) return ERR_ECO_OUTOFMEMORY; \
    converted = (O*)(result + MAX_ORDER); pair.x = x; pair.y = y; \
    status = solve(memory, N##_read, &pair, count, degree, result, method); \
    if (status == ERR_ECO_SUCCESS) { \
        for (i = 0; i <= degree; ++i) { \
            if ((LIMIT) > 0 && magnitude(result[i]) > (LIMIT)) { status = ERR__POLYFIT_NONFINITE; break; } \
            converted[i] = (O)result[i]; \
            if (converted[i] - converted[i] != 0) { status = ERR__POLYFIT_NONFINITE; break; } \
        } \
        if (status == ERR_ECO_SUCCESS) memory->pVTbl->Copy(memory, out, converted, bytes); \
    } \
    memory->pVTbl->Free(memory, result); \
    return status; \
}
API(polyfit_solve, double, double, DoublePair, 1.7976931348623157e308L, floating_input_exact)
API(polyfit_solve_float, float, float, FloatPair, 3.4028234663852886e38L, floating_input_exact)
API(polyfit_solve_long_double, long double, long double, LongDoublePair, 0.0L, floating_input_exact)
API(polyfit_solve_int, int, double, IntPair, 1.7976931348623157e308L, integer_input_exact)
API(polyfit_solve_long, long, long double, LongPair, 0.0L, integer_input_exact)
