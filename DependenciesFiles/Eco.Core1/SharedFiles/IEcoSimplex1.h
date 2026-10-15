/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IEcoSimplex1
 * </summary>
 *
 * <description>
 *   This header describes the interface IEcoSimplex1
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

#ifndef __I_ECO_SIMPLEX_1_H__
#define __I_ECO_SIMPLEX_1_H__

#include "IEcoBase1.h"
#include "IEcoTensor1.h"

#ifndef ECO_SIMPLEX_V1_T_DEFINED

#pragma pack(push, 1)
typedef struct ECO_SIMPLEX_V1_T {
    
    /* ========================================================================== */
    /*   ЧАСТЬ 1: УПРАВЛЕНИЕ И МЕТАДАННЫЕ (Строго 8 байт на любой архитектуре)     */
    /* ========================================================================== */
    uint32_t ref_count;   /* [0-3] Атомарный счетчик ссылок для жизненного цикла узла */
    uint16_t node_flags;  /* [4-5] Флаги формата (SIMPLE_HDR, LINE, TREE, Lock-Free) */
    uint8_t  struct_type; /* [6]   Тип контейнера (ECO_VARIANT_1_MAP_T, LIST_2D_T и др.) */
    uint8_t  layout_align;/* [7]   Выравнивание ячейки в Арене памяти (степень двойки) */

    /* ========================================================================== */
    /*   ЧАСТЬ 2: ТОПОЛОГИЧЕСКАЯ МАТРИЦА СВЯЗЕЙ (16 байт на x86 / 32 байта на x64)*/
    /* ========================================================================== */
    void*    ports[4];    /* Прямые указатели на соседние узлы ECO_SIMPLEX_V1_T */

    /* ========================================================================== */
    /*   ЧАСТЬ 3: РАБОЧАЯ ОБЛАСТЬ ДАННЫХ (Всегда ровно 16 байт)                   */
    /* ========================================================================== */
    ECO_VARIANT_V1_T variant; /* Наш кроссплатформенный Вариант (содержит id и regs) */

} ECO_SIMPLEX_V1_T;
#pragma pack(pop)

/* ========================================================================== */
/*   СИСТЕМНЫЕ ФЛАГИ ФОРМАТА ЯЧЕЙКИ (поле node_flags)                         */
/* ========================================================================== */
#define ECO_SIMPLEX_FLAG_SENTINEL  0x0001  /* Ячейка является Головой-Стражем (Sentinel Node) контейнера */


#pragma pack(push, 1)
typedef struct ECO_SIMPLEX_ITERATOR_V1_T {
    
    /*
     * ABI-СТАБИЛЬНОСТЬ: все адресные поля фиксированы как uint64_t (8 байт)
     * в соответствии с IEcoSimplex.idl — размер структуры всегда 28 байт
     * независимо от разрядности платформы (x86/x64). На 32-битных системах
     * указатель хранится с нулевым расширением до 64 бит.
     */

    /* 1. Хэндл Головы контейнера (Sentinel Node), к которому привязан обход */
    uint64_t  container_head; /* Строго 8 байт на любой архитектуре */
    
    /* 2. Текущий физический КУРСОР — адрес variant внутри Симплекса, на котором стоим */
    uint64_t  current_node;   /* Строго 8 байт на любой архитектуре */
    
    /* 3. Системный маркер типа контейнера (LIST_2D_T, MAP_T, HASHMAP_T) */
    uint16_t  container_type; /* 2 байта */
    
    /* 4. Внутренние служебные битовые флаги навигации */
    uint16_t  state_flags;    /* 2 байта */
    
    /* 5. Локальный контекст/стек возврата для обхода глубоких графов (деревьев поиска) */
    uint64_t  traverse_stack; /* Строго 8 байт на любой архитектуре */

} ECO_SIMPLEX_ITERATOR_V1_T;
#pragma pack(pop)

/* ========================================================================== */
/*   СИСТЕМНЫЕ ПОЗИЦИИ ИНИЦИАЛИЗАЦИИ СИМПЛЕКС-ИТЕРАТОРА (ABI СТАБИЛЬНОСТЬ)     */
/* ========================================================================== */
#define ECO_SIMPLEX_ITER_BEGIN  0   /* Направить курсор на первый элемент графа */
#define ECO_SIMPLEX_ITER_END    1   /* За последний элемент (в Системный NULL) */
#define ECO_SIMPLEX_ITER_CUSTOM 2   /* Выставить курсор на произвольный узел */


/* Указатель на симплекс-ячейку для использования в интерфейсах */
typedef struct ECO_SIMPLEX_V1_T* ECO_SIMPLEX_V1_PTR_t;

#define ECO_SIMPLEX_V1_T_DEFINED
#endif

/* IEcoSimplex1 IID = {4D564FCB-F579-4D91-99B0-C6050A8954B0} */
#ifndef __IID_IEcoSimplex1
static const UGUID IID_IEcoSimplex1 = { 0x01, 0x10, {0x4D, 0x56, 0x4F, 0xCB, 0xF5, 0x79, 0x4D, 0x91, 0x99, 0xB0, 0xC6, 0x05, 0x0A, 0x89, 0x54, 0xB0} };
#endif /* __IID_IEcoSimplex1 */

typedef struct IEcoSimplex1* IEcoSimplex1Ptr_t;

typedef struct IEcoSimplex1VTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoSimplex1Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoSimplex1Ptr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoSimplex1Ptr_t me);

    /* IEcoSimplex1 */
    int16_t (ECOCALLMETHOD *AllocNode)(/* in */ IEcoSimplex1Ptr_t me, /* in */ uint8_t structType, /* out */ ECO_SIMPLEX_V1_T** ppNode);
    int16_t (ECOCALLMETHOD *FreeNode)(/* in */ IEcoSimplex1Ptr_t me, /* in */ ECO_SIMPLEX_V1_T* pNode);
    int16_t (ECOCALLMETHOD *ConnectNodes)(/* in */IEcoSimplex1Ptr_t me, /* in */ ECO_SIMPLEX_V1_T* pSourceNode, /* in */ uint32_t portIndex, /* in */ ECO_SIMPLEX_V1_T* pTargetNode);
    int16_t (ECOCALLMETHOD *DisconnectPort)(/* in */ IEcoSimplex1Ptr_t me, /* in */ ECO_SIMPLEX_V1_T* pNode, /* in */ uint32_t portIndex, /* out */ ECO_SIMPLEX_V1_T** ppOutNode);
    int16_t (ECOCALLMETHOD *SwapPorts)(/* in */ IEcoSimplex1Ptr_t me, /* in */ ECO_SIMPLEX_V1_T* pNode, /* in */ uint32_t portIndexA, /* in */ uint32_t portIndexB);
    int16_t (ECOCALLMETHOD *CreateIterator)(/* in */ IEcoSimplex1Ptr_t me, /* in */ ECO_SIMPLEX_V1_T* pContainerSentinel, /* in */ uint32_t initPosition, /* out */ ECO_SIMPLEX_ITERATOR_V1_T* pIterator);
    int16_t (ECOCALLMETHOD *IteratorNext)(/* in */ IEcoSimplex1Ptr_t me, /* in, out */ ECO_SIMPLEX_ITERATOR_V1_T* pIterator);
    bool_t (ECOCALLMETHOD *IteratorHasNext)(/* in */ IEcoSimplex1Ptr_t me, /* in */ ECO_SIMPLEX_ITERATOR_V1_T* pIterator);

} IEcoSimplex1VTbl, *IEcoSimplex1VTblPtr_t;

interface IEcoSimplex1 {
    struct IEcoSimplex1VTbl *pVTbl;
} IEcoSimplex1;

#endif /* __I_ECO_SIMPLEX_1_H__ */

