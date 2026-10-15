/*
 * Кодировка: UTF-8.
 * Назначение: публичные объявления внутреннего polyfit-ядра.
 * Описание: перегрузки формируются одним generic-шаблоном.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#include "IEcoMemoryAllocator1.h"
#include "Err.h"

#define _REAL double
#define _OUT double
#define _FN polyfit_solve_00000045
#include "polyfit_core_template.h"
#undef _FN
#undef _OUT
#undef _REAL

#define _REAL float
#define _OUT float
#define _FN polyfit_solve_float_00000045
#include "polyfit_core_template.h"
#undef _FN
#undef _OUT
#undef _REAL

#define _REAL long double
#define _OUT long double
#define _FN polyfit_solve_long_double_00000045
#include "polyfit_core_template.h"
#undef _FN
#undef _OUT
#undef _REAL

#define _REAL int
#define _OUT double
#define _FN polyfit_solve_int_00000045
#include "polyfit_core_template.h"
#undef _FN
#undef _OUT
#undef _REAL

#define _REAL long
#define _OUT long double
#define _FN polyfit_solve_long_00000045
#include "polyfit_core_template.h"
#undef _FN
#undef _OUT
#undef _REAL
