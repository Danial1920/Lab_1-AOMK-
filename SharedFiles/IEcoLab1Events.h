#ifndef I_ECO_LAB1_EVENTS_H
#define I_ECO_LAB1_EVENTS_H
#include "IEcoPolynomialMath.h"

static const UGUID IID_IEcoLab1Events = {0x01,0x10,
    {0xb3,0x66,0xce,0x11,0x85,0xaf,0x49,0x4e,0x94,0xdf,0x4e,0xf4,0x4a,0x59,0xc1,0xf3}};
typedef struct IEcoLab1Events IEcoLab1Events;
/* All arguments are borrowed until callback return. Indices are zero-based.
 * method 0 = normal equations, 1 = streaming QR. Coefficients are descending.
 * Notifications run synchronously on the thread calling polyfit. Callback
 * return codes are advisory; a sink cannot cancel a fit or silence its peers. */
typedef struct IEcoLab1EventsVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoLab1Events*, const UGUID*, voidptr_t*);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoLab1Events*);
    uint32_t (ECOCALLMETHOD *Release)(IEcoLab1Events*);
    int16_t (ECOCALLMETHOD *OnFitStarted)(IEcoLab1Events*, IEcoPolynomialMath* source,
        uint32_t method, uint32_t count, uint32_t degree);
    int16_t (ECOCALLMETHOD *OnPivotSelected)(IEcoLab1Events*, IEcoPolynomialMath* source,
        uint32_t column, uint32_t row, long double pivot);
    int16_t (ECOCALLMETHOD *OnGivensRotation)(IEcoLab1Events*, IEcoPolynomialMath* source,
        uint32_t sample, uint32_t column, long double cosine, long double sine);
    int16_t (ECOCALLMETHOD *OnFitFinished)(IEcoLab1Events*, IEcoPolynomialMath* source,
        int16_t status, uint32_t length, const long double* coefficients);
} IEcoLab1EventsVTbl;
struct IEcoLab1Events { IEcoLab1EventsVTbl* pVTbl; };
#endif
