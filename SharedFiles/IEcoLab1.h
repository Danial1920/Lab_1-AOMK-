/* Root identity interface for Eco.Lab1. */
#ifndef I_ECO_LAB1_H
#define I_ECO_LAB1_H
#include "IEcoBase1.h"
static const UGUID IID_IEcoLab1 = {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,0x52}};
typedef struct IEcoLab1 IEcoLab1;
typedef struct IEcoLab1* IEcoLab1Ptr;
typedef struct IEcoLab1VTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoLab1Ptr, const UGUID*, voidptr_t*);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoLab1Ptr);
    uint32_t (ECOCALLMETHOD *Release)(IEcoLab1Ptr);
} IEcoLab1VTbl;
struct IEcoLab1 { IEcoLab1VTbl* pVTbl; };
#endif
