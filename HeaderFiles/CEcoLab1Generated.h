/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#ifndef C_ECO_LAB1_GENERATED_H
#define C_ECO_LAB1_GENERATED_H
#include "IEcoPolynomialMath.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoLab1.h"
#include "IEcoMemoryAllocator1.h"
#if defined(ECO_LIB) || defined(ECO_DLL)
#include "IEcoSystem1.h"
#else
typedef struct IEcoSystem1 IEcoSystem1;
#endif
typedef struct CPolynomialMath CPolynomialMath;
typedef struct CPolynomialXView {
    IEcoCalculatorX iface;
    CPolynomialMath* owner;
} CPolynomialXView;
typedef struct CPolynomialYView {
    IEcoCalculatorY iface;
    CPolynomialMath* owner;
} CPolynomialYView;
struct CPolynomialMath {
    IEcoPolynomialMath math;
    CPolynomialXView calculator_x;
    CPolynomialYView calculator_y;
    uint32_t references;
    uint32_t method;
    IEcoMemoryAllocator1* m_pIMem;
    IEcoSystem1* m_pISys;
    IEcoUnknown* m_pIOuter;
};
#endif
