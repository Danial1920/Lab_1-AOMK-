/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#ifndef POLYFIT_COMPONENT_H
#define POLYFIT_COMPONENT_H
#include "IEcoPolynomialMath.h"
#include "IEcoMemoryAllocator1.h"
int16_t PolyfitComponent_Create_00000045(/* in */ IEcoUnknownPtr_t system, /* in */ IEcoMemoryAllocator1* memory, /* in */ uint32_t method, /* in */ IEcoUnknownPtr_t outer, /* out */ IEcoPolynomialMathPtr* component);
void PolyfitComponent_Destroy_00000045(/* in */ IEcoPolynomialMathPtr component);
#endif
