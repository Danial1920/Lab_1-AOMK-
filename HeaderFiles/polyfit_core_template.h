/*
 * Кодировка: UTF-8.
 * Назначение: единый шаблон объявления типизированного polyfit-ядра.
 * Описание: файл намеренно не содержит include guard и подключается
 * несколько раз через макросы _REAL, _OUT и _FN.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#if !defined(_REAL) || !defined(_OUT) || !defined(_FN)
#error "polyfit_core_template.h requires _REAL, _OUT and _FN"
#endif

int16_t _FN(/* in */ IEcoMemoryAllocator1* memory,
    /* in */ const _REAL* x, /* in */ const _REAL* y,
    /* in */ uint32_t count, /* in */ uint32_t degree,
    /* out */ _OUT* out, /* in */ uint32_t method);
