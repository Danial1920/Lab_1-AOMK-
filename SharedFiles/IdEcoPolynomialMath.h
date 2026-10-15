/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#ifndef __ID_ECO_POLYNOMIAL_MATH_H__
#define __ID_ECO_POLYNOMIAL_MATH_H__

#include "IEcoBase1.h"
#include "IEcoPolynomialMath.h"

/* EcoPolynomialMath CID = {D3B3E2F1-8D21-4A6B-9A40-440000000045} */
static const UGUID CID_EcoPolynomialMath = {
    0x01, 0x10, {0xD3, 0xB3, 0xE2, 0xF1, 0x8D, 0x21, 0x4A, 0x6B,
                 0x9A, 0x40, 0x44, 0x00, 0x00, 0x00, 0x00, 0x45}
};

/* EcoPolynomialMathQR CID = {D3B3E2F1-8D21-4A6B-9A40-440000000046} */
static const UGUID CID_EcoPolynomialMathQR = {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,0x46}};
IEcoComponentFactory* GetPolynomialFactory_00000045(void);
IEcoComponentFactory* GetPolynomialQRFactory_00000046(void);

#define GetIEcoComponentFactoryPtr GetPolynomialFactory_00000045
#define GetPolyfitQRFactory GetPolynomialQRFactory_00000046
#endif
