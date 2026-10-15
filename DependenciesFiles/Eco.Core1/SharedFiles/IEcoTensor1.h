/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IEcoTensor1
 * </summary>
 *
 * <description>
 *   This header describes the interface IEcoTensor1
 * </description>
 *
 * <reference>
 *
 * </reference>
 *
 * <author>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </author>
 *
 */

#ifndef __I_ECO_TENSOR_1_H__
#define __I_ECO_TENSOR_1_H__

#include "IEcoBase1.h"

#ifndef ECO_TENSOR_V1_T_DEFINED

/* 1. Базовые константы (значения, которые мы ожидаем увидеть в magic) */
#define ECO_TENSOR_MAGIC_LE 0xFFFEFD01  /* Little-Endian (младший байт 01 в конце) */
#define ECO_TENSOR_MAGIC_BE 0x01FDFEFF  /* Big-Endian (01 в начале) */
#define ECO_TENSOR_MAGIC_ME 0xFD01FFFE  /* Middle-Endian (PDP-11 стиль) */

/* 2. Проверка порядка байт ТЕНЗОРА (как он записан в структуре) */
#define ECO_TENSOR_IS_LE(t) ((t)->magic == ECO_TENSOR_MAGIC_LE)
#define ECO_TENSOR_IS_BE(t) ((t)->magic == ECO_TENSOR_MAGIC_BE)
#define ECO_TENSOR_IS_ME(t) ((t)->magic == ECO_TENSOR_MAGIC_ME)

/* 3. Определение нативной архитектуры системы (Runtime check) */
/* Создаем временную переменную и смотрим на её первый байт */

static const uint32_t __eco_endian_test = 0x01020304;
#define ECO_SYSTEM_IS_LITTLE_ENDIAN() (*((const uint8_t*)&__eco_endian_test) == 0x04)
#define ECO_SYSTEM_IS_BIG_ENDIAN()    (*((const uint8_t*)&__eco_endian_test) == 0x01)
#define ECO_SYSTEM_IS_MIDDLE_ENDIAN() (*((const uint8_t*)&__eco_endian_test) == 0x02)

/* 4. Получение версии из magic (всегда первый байт физической последовательности) */
/* Версия 01 всегда лежит по самому первому адресу, независимо от порядка байт системы */
#define ECO_TENSOR_GET_VERSION(t) (*((uint8_t*)&((t)->magic)))

/* ========================================================================== */
/*   ГРУППА 1: Топология и разреженность (Биты 0-3, маска 0x000F)            */
/* ========================================================================== */
#define ECO_TENSOR_LAYOUT_MASK          0x000F

#define ECO_TENSOR_LAYOUT_DENSE         0x0000  /* Полный многомерный плотный тензор */
#define ECO_TENSOR_LAYOUT_CSR           0x0001  /* Разреженная матрица: Compressed Sparse Row (для лексера) */
#define ECO_TENSOR_LAYOUT_COO           0x0002  /* Универсальный N-мерный координатный формат [nnz, ndims] */
#define ECO_TENSOR_LAYOUT_CSF           0x0003  /* Compressed Sparse Fiber (N-мерный аналог CSR) */
#define ECO_TENSOR_LAYOUT_SPARSE_24     0x0004  /* Структурированная ИИ-разреженность (2 из 4, NVIDIA Ampere+) */
#define ECO_TENSOR_LAYOUT_CSC           0x0005  /* Разреженная матрица: Compressed Sparse Column */
#define ECO_TENSOR_LAYOUT_ELLPACK       0x0006  /* Формат ELL: фиксированное число ненулевых элементов в строке */

/* ========================================================================== */
/*   ГРУППА 2: Адресация, бинарный формат и права (Биты 4-7, маска 0x00F0)    */
/* ========================================================================== */
#define ECO_TENSOR_FLAG_RELATIVE_PTRS   0x0010  /* КРИТИЧЕСКИЙ ФЛАГ: указатели являются смещениями (int64_t offset) */
#define ECO_TENSOR_FLAG_SIMPLE_HDR      0x0020  /* Бинарный файл использует упакованный 12-байтовый заголовок */
#define ECO_TENSOR_FLAG_FOREIGN         0x0040  /* Данные внешние (SaC / сторонние). Деструктор НЕ вызывает free() */
#define ECO_TENSOR_FLAG_READONLY        0x0080  /* Память только для чтения (Memory Map / Срез без мутации) */

/* ========================================================================== */
/*   ГРУППА 3: Аппаратное размещение и оптимизация (Биты 8-11, маска 0x0F00)  */
/* ========================================================================== */
#define ECO_TENSOR_FLAG_PINNED          0x0100  /* Память хоста заблокирована (Page-Locked) для сверхбыстрого DMA */
#define ECO_TENSOR_FLAG_DEVICE          0x0200  /* Данные на ускорителе (CUDA Pointer / Vulkan buffer) */
#define ECO_TENSOR_FLAG_SHARED          0x0400  /* Межпроцессная разделяемая память (IPC Shared Memory Buffer) */
#define ECO_TENSOR_FLAG_ALIGNED_PAGE    0x0800  /* Физический адрес данных выровнен по системной странице (4КБ/2МБ) */

/* ========================================================================== */
/*   ГРУППА 4: Потоковая модель и состояние расчетов (Биты 12-15, маска 0xF000)*/
/* ========================================================================== */
#define ECO_TENSOR_FLAG_VIEW            0x1000  /* Срез / Мягкая копия (Тензор разделяет память data_ptr с другим тензором) */
#define ECO_TENSOR_FLAG_DIRTY           0x2000  /* Данные изменены на CPU, но не синхронизированы с кешем GPU/диском */
#define ECO_TENSOR_FLAG_CONTIGUOUS      0x4000  /* Гарантированная непрерывность памяти по всем осям (нет кастомных strides) */
#define ECO_TENSOR_FLAG_LAZY            0x8000  /* Флаг отложенного ленивого вычисления */


/* ========================================================================== */
/*   Вспомогательные макросы быстрой проверки состояния (Предикаты)           */
/* ========================================================================== */
#define ECO_TENSOR_GET_LAYOUT(t)        ((t)->flags & ECO_TENSOR_LAYOUT_MASK)
#define ECO_TENSOR_IS_DENSE(t)          (ECO_TENSOR_GET_LAYOUT(t) == ECO_TENSOR_LAYOUT_DENSE)
#define ECO_TENSOR_IS_SPARSE(t)         (ECO_TENSOR_GET_LAYOUT(t) >= ECO_TENSOR_LAYOUT_CSR)

/* Предикат проверки формата физического заголовка (8 байт против 16 байт) */
#define ECO_TENSOR_IS_SIMPLE_HDR(t)     ((t)->flags & ECO_TENSOR_FLAG_SIMPLE_HDR)

#define ECO_TENSOR_CAN_MUTATE(t)        (!((t)->flags & ECO_TENSOR_FLAG_READONLY))
#define ECO_TENSOR_IS_ON_DEVICE(t)      ((t)->flags & ECO_TENSOR_FLAG_DEVICE)
#define ECO_TENSOR_IS_THREADSAFE(t)     ((t)->flags & ECO_TENSOR_FLAG_THREADSAFE)


/* Структура тензора первой версии */

#pragma pack(push, 1)

/* Наш выверенный заголовок (Строго 16 байт) */
typedef struct ECO_TENSOR_META_V1_T {
    uint32_t total_elements; /* [0-3]   Общее количество элементов */
    uint32_t ndims;          /* [4-7]   Количество измерений */
    uint16_t flags;          /* [8-9]   Флаги (Сюда добавляем ECO_TENSOR_FL_RELATIVE_PTRS) */
    uint16_t data_type;      /* [10-11] Тип данных элементов */
    uint16_t shape_type;     /* [12-13] Тип данных элементов shape */
    uint8_t  data_align;     /*    Выравнивание данных (степень двойки) */
    uint8_t  shape_align;    /*    Выравнивание shape (степень двойки) */
} ECO_TENSOR_META_V1_T;

/* Кроссплатформенный Рантайм-Дескриптор Тензора (Строго 64 байта, L1-выровненный) */
typedef struct ECO_RUNTIME_TENSOR_V1_T {
    
    ECO_TENSOR_META_V1_T hdr;       /* [0-15]  Метаданные (16 байт) */
    
    EcoLink64_t shape_ptr;             /* [16-23] Абсолютный адрес ИЛИ смещение к массиву Shape */
    EcoLink64_t strides_ptr;           /* [24-31] Абсолютный адрес ИЛИ смещение к массиву Strides */
    EcoLink64_t data_ptr;              /* [32-39] Абсолютный адрес ИЛИ смещение к массиву Данных */
    EcoLink64_t sparse_indices;        /* [40-47] CSR/COO Индексы */
    EcoLink64_t sparse_indptr;         /* [48-55] CSR Смещения строк */
    
    EcoLink64_t destroy_callback;      /* [56-63] Деструктор (для монолитного тензора равен NULL) */

} ECO_RUNTIME_TENSOR_V1_T;

#pragma pack(pop)

#define ECO_TENSOR_V1_T_DEFINED
#endif

/* IEcoTensor1 IID = {00000000-0000-0000-0000-0001FDFEFF} */
#ifndef __IID_IEcoTensor1
static const UGUID IID_IEcoTensor1 = { 0x01, 0x10, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFD, 0xFE, 0xFF} };
#endif /* __IID_IEcoTensor1 */

typedef struct IEcoTensor1* IEcoTensor1Ptr_t;

typedef struct IEcoTensor1VTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoTensor1Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoTensor1Ptr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoTensor1Ptr_t me);

} IEcoTensor1VTbl, *IEcoTensor1VTblPtr_t;

interface IEcoTensor1 {
    struct IEcoTensor1VTbl *pVTbl;
} IEcoTensor1;

#endif /* __I_ECO_TENSOR_1_H__ */

