/* Checked extension; does not change the educational X/Y ABI. */
#ifndef I_ECO_CALCULATOR_SAFE_H
#define I_ECO_CALCULATOR_SAFE_H
#include "IEcoBase1.h"
static const UGUID IID_IEcoCalculatorSafe = {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,0x54}};
#define ERR_ECO_CALCULATOR_ZERO ((int16_t)1101)
#define ERR_ECO_CALCULATOR_OVERFLOW ((int16_t)1102)
typedef struct IEcoCalculatorSafe IEcoCalculatorSafe;
typedef struct IEcoCalculatorSafeVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoCalculatorSafe*, const UGUID*, voidptr_t*);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoCalculatorSafe*);
    uint32_t (ECOCALLMETHOD *Release)(IEcoCalculatorSafe*);
    int16_t (ECOCALLMETHOD *TryDivision)(IEcoCalculatorSafe*, int16_t, int16_t, int16_t*);
} IEcoCalculatorSafeVTbl;
struct IEcoCalculatorSafe { IEcoCalculatorSafeVTbl* pVTbl; };
#endif
