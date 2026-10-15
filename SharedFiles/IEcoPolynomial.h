/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#ifndef I_ECO_POLYNOMIAL_H
#define I_ECO_POLYNOMIAL_H
#include "IEcoBase1.h"
/* IEcoPolynomial IID = {D3B3E2F1-8D21-4A6B-9A40-440000000047} */
static const UGUID IID_IEcoPolynomial = {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,0x47}};
typedef struct IEcoPolynomial IEcoPolynomial;
typedef struct IEcoPolynomialVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoPolynomial* me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoPolynomial* me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoPolynomial* me);
    int16_t (ECOCALLMETHOD *get_Degree)(/* in */ IEcoPolynomial* me, /* out */ uint32_t* degree);
    int16_t (ECOCALLMETHOD *get_Coefficients)(/* in */ IEcoPolynomial* me, /* in */ uint32_t capacity, /* out */ double* coefficients);
    int16_t (ECOCALLMETHOD *Evaluate)(/* in */ IEcoPolynomial* me, /* in */ double x, /* out */ double* value);
} IEcoPolynomialVTbl;
struct IEcoPolynomial { IEcoPolynomialVTbl* pVTbl; };
#endif
