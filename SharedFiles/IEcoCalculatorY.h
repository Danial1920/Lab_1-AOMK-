/* IEcoCalculatorY: calculator operations exposed by ACOM Lab 2. */
#ifndef I_ECO_CALCULATOR_Y_H
#define I_ECO_CALCULATOR_Y_H
#include "IEcoBase1.h"
static const UGUID IID_IEcoCalculatorY = {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,0x51}};
typedef struct IEcoCalculatorY IEcoCalculatorY;
typedef struct IEcoCalculatorY* IEcoCalculatorYPtr;
typedef struct IEcoCalculatorYVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoCalculatorYPtr, const UGUID*, voidptr_t*);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoCalculatorYPtr);
    uint32_t (ECOCALLMETHOD *Release)(IEcoCalculatorYPtr);
    double (ECOCALLMETHOD *Multiplication)(IEcoCalculatorYPtr, double, double);
    double (ECOCALLMETHOD *Division)(IEcoCalculatorYPtr, double, double);
    int16_t (ECOCALLMETHOD *TryDivision)(IEcoCalculatorYPtr, double, double, double*);
} IEcoCalculatorYVTbl;
struct IEcoCalculatorY { IEcoCalculatorYVTbl* pVTbl; };
#endif
