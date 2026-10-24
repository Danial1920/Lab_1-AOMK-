/* IEcoCalculatorX: calculator operations exposed by ACOM Lab 2. */
#ifndef I_ECO_CALCULATOR_X_H
#define I_ECO_CALCULATOR_X_H
#include "IEcoBase1.h"
static const UGUID IID_IEcoCalculatorX = {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,0x50}};
typedef struct IEcoCalculatorX IEcoCalculatorX;
typedef struct IEcoCalculatorX* IEcoCalculatorXPtr;
typedef struct IEcoCalculatorXVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoCalculatorXPtr, const UGUID*, voidptr_t*);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoCalculatorXPtr);
    uint32_t (ECOCALLMETHOD *Release)(IEcoCalculatorXPtr);
    double (ECOCALLMETHOD *Addition)(IEcoCalculatorXPtr, double, double);
    double (ECOCALLMETHOD *Subtraction)(IEcoCalculatorXPtr, double, double);
} IEcoCalculatorXVTbl;
struct IEcoCalculatorX { IEcoCalculatorXVTbl* pVTbl; };
#endif
