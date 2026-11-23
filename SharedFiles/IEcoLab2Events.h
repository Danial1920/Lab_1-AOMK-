#ifndef I_ECO_LAB2_EVENTS_H
#define I_ECO_LAB2_EVENTS_H
#include "IEcoBase1.h"

static const UGUID IID_IEcoLab2Events = {0x01,0x10,
    {0xAB,0x50,0xC6,0xC5,0xF6,0xF9,0x45,0x7C,0xA8,0x34,0xBB,0x95,0xC5,0xD4,0xF7,0x69}};
typedef struct IEcoLab2Events IEcoLab2Events;
typedef struct IEcoLab2EventsVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoLab2Events*, const UGUID*, voidptr_t*);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoLab2Events*);
    uint32_t (ECOCALLMETHOD *Release)(IEcoLab2Events*);
    int16_t (ECOCALLMETHOD *OnHornerStarted)(IEcoLab2Events*, const int16_t* coefficients, uint32_t count, int16_t x);
    int16_t (ECOCALLMETHOD *OnHornerStep)(IEcoLab2Events*, uint32_t index, int32_t product, int16_t coefficient, int32_t accumulator);
    /* On error, accumulator is the last completed partial value. */
    int16_t (ECOCALLMETHOD *OnHornerFinished)(IEcoLab2Events*, int32_t accumulator, int16_t status);
} IEcoLab2EventsVTbl;
struct IEcoLab2Events { IEcoLab2EventsVTbl* pVTbl; };
#endif
