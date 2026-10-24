/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#ifndef POLYFIT_BUS_H
#define POLYFIT_BUS_H
#include "IEcoInterfaceBus1.h"
#include "IEcoPolynomialMath.h"
#include "IdEcoPolynomialMath.h"
#include "IdEcoLab2.h"
int16_t PolyfitComponent_RegisterOnBus(IEcoInterfaceBus1Ptr_t bus);
int16_t PolyfitComponent_QueryFromBus(IEcoInterfaceBus1Ptr_t bus, IEcoPolynomialMathPtr* component);
int16_t EcoLab2_RegisterOnBus(IEcoInterfaceBus1Ptr_t bus);
#endif
