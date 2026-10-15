/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   macros.h
 * </сводка>
 *
 * <описание>
 *   Данный заголовок описывает макросы
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2016 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#ifndef __ECO_MACROS_H__
#define __ECO_MACROS_H__

#define COMP_CALL(comp, method, ...) comp->pVTbl->method(comp, __VA_ARGS__)
#define COMP_CALL_(comp, method) comp->pVTbl->method

#define I(comp) comp->pVTbl->

#define CONST_CAST(T,EXPR)        ((T)(EXPR))
#define REINTERPRET_CAST(T,EXPR)  ((T)(EXPR))
#define INTEGER_CAST(T,EXPR)      ((T)(EXPR))
#define POINTER_CAST(T,EXPR)      ((T)(EXPR))

#ifndef OFFSET_OF
#define OFFSET_OF(T, MEMBER)      ((size_t)(char_t *)&((T *)0)->MEMBER)
#endif

#define STATIC_CAST(T, EXPR, TYPE, MEMBER) \
    POINTER_CAST(T, (char_t*)(EXPR) - OFFSET_OF(TYPE, MEMBER))

#endif /* __ECO_MACROS_H__ */
