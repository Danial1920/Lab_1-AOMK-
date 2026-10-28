/* Polynomial evaluation interface for the external Eco.Lab2 component. */
#ifndef I_ECO_LAB2_H
#define I_ECO_LAB2_H
#include "IEcoBase1.h"
static const UGUID IID_IEcoLab2 = {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,0x55}};
typedef struct IEcoLab2 IEcoLab2;
typedef struct IEcoLab2* IEcoLab2Ptr;
typedef struct IEcoLab2VTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoLab2Ptr, const UGUID*, voidptr_t*);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoLab2Ptr);
    uint32_t (ECOCALLMETHOD *Release)(IEcoLab2Ptr);
    int16_t (ECOCALLMETHOD *Horner)(IEcoLab2Ptr, const int16_t*, uint32_t, int16_t, int32_t*);
} IEcoLab2VTbl;
struct IEcoLab2 { IEcoLab2VTbl* pVTbl; };
#endif
