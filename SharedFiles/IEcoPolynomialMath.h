/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#ifndef I_ECO_POLYNOMIAL_MATH_H
#define I_ECO_POLYNOMIAL_MATH_H
#include "IEcoBase1.h"
#include "IEcoPolynomial.h"
#include "Err.h"
/* IEcoPolynomialMath IID = {D3B3E2F1-8D21-4A6B-9A40-440000000044} */
static const UGUID IID_IEcoPolynomialMath = {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,0x44}};
typedef struct IEcoPolynomialMath* IEcoPolynomialMathPtr;
typedef struct EcoVectorView {
    const double* data;
    uint32_t length;
} EcoVectorView;
typedef struct IEcoPolynomialMathVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoPolynomialMathPtr me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoPolynomialMathPtr me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoPolynomialMathPtr me);
    /* Buffers contain degree+1 coefficients in descending powers.
       Output buffers must be disjoint from both input arrays. */
    int16_t (ECOCALLMETHOD *polyfit)(/* in */ IEcoPolynomialMathPtr me, /* in */ const double* x, /* in */ const double* y, /* in */ uint32_t count, /* in */ uint32_t degree, /* out */ double* coefficients);
    int16_t (ECOCALLMETHOD *polyfitf)(/* in */ IEcoPolynomialMathPtr me, /* in */ const float* x, /* in */ const float* y, /* in */ uint32_t count, /* in */ uint32_t degree, /* out */ float* coefficients);
    int16_t (ECOCALLMETHOD *polyfitl)(/* in */ IEcoPolynomialMathPtr me, /* in */ const long double* x, /* in */ const long double* y, /* in */ uint32_t count, /* in */ uint32_t degree, /* out */ long double* coefficients);
    int16_t (ECOCALLMETHOD *polyfiti)(/* in */ IEcoPolynomialMathPtr me, /* in */ const int* x, /* in */ const int* y, /* in */ uint32_t count, /* in */ uint32_t degree, /* out */ double* coefficients);
    int16_t (ECOCALLMETHOD *polyfit_long)(/* in */ IEcoPolynomialMathPtr me, /* in */ const long* x, /* in */ const long* y, /* in */ uint32_t count, /* in */ uint32_t degree, /* out */ long double* coefficients);
    int16_t (ECOCALLMETHOD *Add)(/* in */ IEcoPolynomialMathPtr me, /* in */ double x, /* in */ double y, /* out */ double* value);
    int16_t (ECOCALLMETHOD *Subtract)(/* in */ IEcoPolynomialMathPtr me, /* in */ double x, /* in */ double y, /* out */ double* value);
    int16_t (ECOCALLMETHOD *FitVector)(/* in */ IEcoPolynomialMathPtr me, /* in */ const EcoVectorView* x, /* in */ const EcoVectorView* y, /* in */ uint32_t degree, /* out */ double* coefficients);
    int16_t (ECOCALLMETHOD *new_Polynomial)(/* in */ IEcoPolynomialMathPtr me, /* in */ const double* x, /* in */ const double* y, /* in */ uint32_t count, /* in */ uint32_t degree, /* out */ IEcoPolynomial** polynomial);
} IEcoPolynomialMathVTbl;
interface IEcoPolynomialMath { IEcoPolynomialMathVTbl* pVTbl; } IEcoPolynomialMath;
#endif
