/* Internal declarations for Lab 2 calculator components. */
#ifndef C_ECO_LAB2_H
#define C_ECO_LAB2_H
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoLab1.h"
#include "IEcoLab2.h"
#include "IEcoMemoryAllocator1.h"
#include "IEcoInterfaceBus1.h"
#if defined(ECO_LIB) || defined(ECO_DLL)
#include "IEcoSystem1.h"
#else
typedef struct IEcoSystem1 IEcoSystem1;
#endif
typedef struct CCalculatorComponent CCalculatorComponent;
typedef struct CCalculatorXView {
    IEcoCalculatorX iface;
    CCalculatorComponent* owner;
} CCalculatorXView;
typedef struct CCalculatorYView {
    IEcoCalculatorY iface;
    CCalculatorComponent* owner;
} CCalculatorYView;
struct CCalculatorComponent {
    CCalculatorXView x;
    CCalculatorYView y;
    uint32_t references;
    uint32_t kind;
    IEcoMemoryAllocator1* memory;
    IEcoUnknown* outer;
};
typedef struct CEcoLab2Component {
    IEcoLab2 lab2;
    uint32_t references;
    IEcoMemoryAllocator1* memory;
    IEcoSystem1* system;
    IEcoLab1* lab1;
    IEcoCalculatorX* calculator_x;
    IEcoCalculatorY* calculator_y;
    IEcoCalculatorX* calculator_c;
    IEcoCalculatorY* calculator_d;
    IEcoCalculatorX* calculator_b;
    IEcoCalculatorY* calculator_e;
} CEcoLab2Component;
int16_t Calculator_Create(IEcoMemoryAllocator1* memory, uint32_t kind,
    IEcoUnknown* outer, CCalculatorComponent** result);
void Calculator_Destroy(CCalculatorComponent* component);
CCalculatorComponent* Calculator_FromX(IEcoCalculatorX* value);
CCalculatorComponent* Calculator_FromY(IEcoCalculatorY* value);
int16_t CalculatorComponents_RegisterOnBus(IEcoInterfaceBus1Ptr_t bus);
#endif
