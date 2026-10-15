/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#ifndef POLYFIT_ERR_H
#define POLYFIT_ERR_H
#include "IEcoBase1.h"
#define ERR__POLYFIT_ARGUMENT ((int16_t)1001)
#define ERR__POLYFIT_SINGULAR ((int16_t)1002)
#define ERR__POLYFIT_NONFINITE ((int16_t)1003)
#define ERR__POLYFIT_BUFFER ((int16_t)1004)
#define ECO_POLYFIT_MAX_DEGREE 16u
#define ECO_POLYFIT_MAX_COUNT 4096u
#endif
