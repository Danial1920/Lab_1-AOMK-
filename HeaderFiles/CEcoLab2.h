/* Internal declarations for the Lab 2 composition graph. */
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
struct IEcoSystem1;
#endif
#define CALCULATOR_A 0u
#define CALCULATOR_B 1u
#define CALCULATOR_C 2u
#define CALCULATOR_D 3u
#define CALCULATOR_E 4u
typedef struct CCalculatorComponent CCalculatorComponent;
typedef struct CCalculatorXView {
    IEcoCalculatorX iface;
    CCalculatorComponent* owner;
} CCalculatorXView;
typedef struct CCalculatorYView {
    IEcoCalculatorY iface;
    CCalculatorComponent* owner;
} CCalculatorYView;
typedef struct CCalculatorUnknownView {
    IEcoUnknown iface;
    CCalculatorComponent* owner;
} CCalculatorUnknownView;
struct CCalculatorComponent {
    CCalculatorUnknownView inner_unknown;
    CCalculatorXView x;
    CCalculatorYView y;
    uint32_t references;
    uint32_t kind;
    IEcoMemoryAllocator1* memory;
    IEcoUnknown* outer;
    IEcoCalculatorX* included_x;
    IEcoCalculatorY* included_y;
    IEcoCalculatorX* included_subtractor;
    IEcoUnknown* aggregated_inner;
    IEcoCalculatorX* aggregated_x; /* Borrowed; the inner unknown owns its lifetime. */
};
typedef struct CEcoLab2Component CEcoLab2Component;
typedef struct CEcoLab2YView {
    IEcoCalculatorY iface;
    CEcoLab2Component* owner;
} CEcoLab2YView;
struct CEcoLab2Component {
    IEcoLab2 lab2;
    CEcoLab2YView calculator_y;
    uint32_t references;
    IEcoMemoryAllocator1* memory;
    struct IEcoSystem1* system;
    IEcoUnknown* lab1_inner;
    IEcoCalculatorY* included_e;
};
int16_t Calculator_Create(IEcoMemoryAllocator1* memory, uint32_t kind,
    IEcoUnknown* outer, CCalculatorComponent** result);
int16_t Calculator_CreateOnBus(IEcoMemoryAllocator1* memory, uint32_t kind,
    IEcoUnknown* outer, IEcoInterfaceBus1* bus, CCalculatorComponent** result);
int16_t CalculatorComponents_RegisterOnBus(IEcoInterfaceBus1Ptr_t bus);
#endif
