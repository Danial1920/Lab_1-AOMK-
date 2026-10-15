/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   ErrEcoCodes
 * </сводка>
 *
 * <описание>
 *   Данный заголовок описывает коды ошибок
 * </описание>
 *
 * <ссылка>
 *
 * </ссылка>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#ifndef __ERR_ECO_CODES_H__
#define __ERR_ECO_CODES_H__

/*
 * <код ошибки>
 *   ERR_ECO_SUCCESS 
 * </код ошибки>
 *
 * <описание>
 *   Выполнено успешно.
 * </описание>
 */
#define ERR_ECO_SUCCESS                 ((int16_t)0x0000)
#define ERR_ECO_OK                      ERR_ECO_SUCCESS 

/*
 * <код ошибки>
 *   ERR_ECO_USER
 * </код ошибки>
 *
 * <описание>
 *   Коды ошибок определенные пользователем.
 * </описание>
 */
#define ERR_ECO_USER                    ((int16_t)0x0002)

/*
 * <код ошибки>
 *   ERR_ECO_UNEXPECTED
 * </код ошибки>
 *
 * <описание>
 *   Непредвиденное условие. Катастрофический сбой.
 * </описание>
 */
#define ERR_ECO_UNEXPECTED              ((int16_t)0xFFFF)

/*
 * <код ошибки>
 *   ERR_ECO_POINTER
 * </код ошибки>
 *
 * <описание>
 *   Было передано неправильное значение указателя.
 * </описание>
 */
#define ERR_ECO_POINTER                 ((int16_t)0xFFEE)

/*
 * <код ошибки>
 *   ERR_ECO_NOINTERFACE
 * </код ошибки>
 *
 * <описание>
 *  Такой интерфейс не поддерживается.
 * </описание>
 */
#define ERR_ECO_NOINTERFACE             ((int16_t)0xFFED)

/*
 * <код ошибки>
 *   ERR_ECO_NOSYSTEM
 * </код ошибки>
 *
 * <описание>
 *  Системный интерфейс не поддерживается.
 * </описание>
 */
#define ERR_ECO_NOSYSTEM                ((int16_t)0xFFEC)

/*
 * <код ошибки>
 *   ERR_ECO_NOBUS
 * </код ошибки>
 *
 * <описание>
 *  Интерфейсная шина не поддерживается.
 * </описание>
 */
#define ERR_ECO_NOBUS                   ((int16_t)0xFFEB)

/*
 * <код ошибки>
 *   ERR_ECO_NOAGGREGATION
 * </код ошибки>
 *
 * <описание>
 *  Агрегирование не поддерживается.
 * </описание>
 */
#define ERR_ECO_NOAGGREGATION           ((int16_t)0xFFEA)

/*
 * <код ошибки>
 *   ERR_ECO_COMPONENT_NOTFOUND
 * </код ошибки>
 *
 * <описание>
 *  Компонент не найден.
 * </описание>
 */
#define ERR_ECO_COMPONENT_NOTFOUND      ((int16_t)0xFFE9)

/*
 * <код ошибки>
 *   ERR_ECO_OUTOFMEMORY
 * </код ошибки>
 *
 * <описание>
 *  Сбой при попытке выделения памяти.
 * </описание>
 */
#define ERR_ECO_OUTOFMEMORY             ((int16_t)0xFFE8)

/*
 * <код ошибки>
 *   ERR_ECO_MEMMGR_NOTREG
 * </код ошибки>
 *
 * <описание>
 *  Компонент управления памятью не зарегистрирован.
 * </описание>
 */
#define ERR_ECO_MEMMGR_NOTREG           ((int16_t)0xFFE7)

/*
 * <код ошибки>
 *   ERR_ECO_GET_MEMORY_ALLOCATOR
 * </код ошибки>
 *
 * <описание>
 *  Сбой с получением распределителя памяти.
 * </описание>
 */
#define ERR_ECO_GET_MEMORY_ALLOCATOR    ((int16_t)0xFFE6)

/*
 * <код ошибки>
 *   ERR_ECO_FAIL
 * </код ошибки>
 *
 * <описание>
 *   Неопознанная ошибка.
 * </описание>
 */
#define ERR_ECO_FAIL                    ((int16_t)0xFFE5)

/*
 * <код ошибки>
 *   ERR_ECO_NOTIMPL
 * </код ошибки>
 *
 * <описание>
 *   Не реализовано.
 * </описание>
 */
#define ERR_ECO_NOTIMPL                 ((int16_t)0xFFE4)

/*
 * <код ошибки>
 *   ERR_ECO_INVALIDARG
 * </код ошибки>
 *
 * <описание>
 *   Один или несколько аргументов являются недопустимыми.
 * </описание>
 */
#define ERR_ECO_INVALIDARG               ((int16_t)0xFFE3)

/*
 * <код ошибки>
 *   ERR_ECO_INVALID_ARGUMENT
 * </код ошибки>
 *
 * <описание>
 *   Синоним ERR_ECO_INVALIDARG: один или несколько аргументов являются недопустимыми
 *   (например, хэндл не является Головой-Стражем контейнера Simplex).
 * </описание>
 */
#define ERR_ECO_INVALID_ARGUMENT         ERR_ECO_INVALIDARG

/*
 * <код ошибки>
 *   ERR_ECO_FILEMGR_NOTREG
 * </код ошибки>
 *
 * <описание>
 *  Компонент управления файловой системой не зарегистрирован.
 * </описание>
 */
#define ERR_ECO_FILEMGR_NOTREG           ((int16_t)0xFFE2)

/*
 * <код ошибки>
 *   ERR_ECO_NETMGR_NOTREG
 * </код ошибки>
 *
 * <описание>
 *  Компонент управления сетью не зарегистрирован.
 * </описание>
 */
#define ERR_ECO_NETMGR_NOTREG           ((int16_t)0xFFE1)


/*
 * <код ошибки>
 *   ERR_ECO_INDEX_OUT_OF_BOUNDS
 * </код ошибки>
 *
 * <описание>
 *  Индекс выходит за границы допустимого диапазона контейнера.
 * </описание>
 */
#define ERR_ECO_INDEX_OUT_OF_BOUNDS      ((int16_t)0xFFE0)

/*
 * <код ошибки>
 *   ERR_ECO_VALUE_NOT_FOUND
 * </код ошибки>
 *
 * <описание>
 *  Запрошенное значение не найдено в контейнере.
 * </описание>
 */
#define ERR_ECO_VALUE_NOT_FOUND          ((int16_t)0xFFDF)

#endif /* __ERR_ECO_CODES_H__ */
