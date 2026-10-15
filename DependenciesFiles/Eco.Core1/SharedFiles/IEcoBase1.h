/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   IEcoBase1
 * </сводка>
 *
 * <описание>
 *   Данный заголовок описывает базовые определения данных
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2016 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#ifndef __I_ECO_BASE_1_H__
#define __I_ECO_BASE_1_H__

/* Поддержка зависимостей */
#include "ErrEcoCodes.h"
#include "depend.h"

#ifndef INTERFACE_DEFINED
#undef interface
#define interface typedef struct
#define INTERFACE_DEFINED
#endif

#ifndef UGUID_DEFINED
typedef struct UGUID UGUID;
#define UGUID_DEFINED
#endif

#ifndef ECO_GUID128_DEFINED
typedef struct UGUID {
    byte_t Preamble;
    byte_t Length;
    byte_t Data[16];
} GUID128;
#define ECO_GUID128_DEFINED
#endif


#pragma pack(push, 1)

/* 
 * EcoLink64_t — Кроссплатформенная 64-битная универсальная связь (Строго 8 байт).
 * В зависимости от контекста и флагов интерпретируется либо как прямой адрес в RAM, 
 * либо как знаковое смещение (offset) в байтах для Zero-Copy сериализации.
 */
typedef union EcoLink64_t {
    void*    addr;      /* Прямой адрес в памяти (используется в динамическом рантайме) */
    int64_t  offset;    /* Относительное смещение в байтах от адреса САМОГО поля */
    uint64_t raw64;     /* Аппаратный фиксатор размера для 32/64-битных систем */
} EcoLink64_t;

#pragma pack(pop)

#ifndef ECO_VARIANT_V1_T_DEFINED

/* --- Типы данных --- */
typedef enum ECO_VARIANT_1_TYPE {
    ECO_VARIANT_1_EMPTY_T        = 0x00, 
    ECO_VARIANT_1_NULL_T         = 0x01, 

    /* Целые беззнаковые */
    ECO_VARIANT_1_UINT8_T        = 0x02, 
    ECO_VARIANT_1_UINT16_T       = 0x03, 
    ECO_VARIANT_1_UINT32_T       = 0x04, 
    ECO_VARIANT_1_UINT64_T       = 0x05,
    ECO_VARIANT_1_UINT128_T      = 0x06,

    /* Целые знаковые */
    ECO_VARIANT_1_INT8_T         = 0x07, 
    ECO_VARIANT_1_INT16_T        = 0x08, 
    ECO_VARIANT_1_INT32_T        = 0x09, 
    ECO_VARIANT_1_INT64_T        = 0x0A,
    ECO_VARIANT_1_INT128_T       = 0x0B,

    /* Сверхбольшие целые */
    ECO_VARIANT_1_UINT256_T      = 0x0C,
    ECO_VARIANT_1_INT256_T       = 0x0D,
    ECO_VARIANT_1_UINT512_T      = 0x0E,
    ECO_VARIANT_1_INT512_T       = 0x0F,

    /* Плавающая точка */
    ECO_VARIANT_1_FLOAT16_T      = 0x20,
    ECO_VARIANT_1_BFLOAT16_T     = 0x21,
    ECO_VARIANT_1_FLOAT32_T      = 0x22, 
    ECO_VARIANT_1_FLOAT64_T      = 0x23,
    ECO_VARIANT_1_FLOAT128_T     = 0x24,

    /* Новые стандарты FP8 (8-битная плавающая точка) */
    ECO_VARIANT_1_FLOAT8_E4M3_T  = 0x25, /* Высокая точность: 1 бит знак, 4 экспонента, 3 мантисса */
    ECO_VARIANT_1_FLOAT8_E5M2_T  = 0x26, /* Высокий диапазон: 1 бит знак, 5 экспонента, 2 мантисса */

    /* Сверхточная плавающая точка */
    ECO_VARIANT_1_FLOAT256_T     = 0x27, /* Octuple precision */

    /* Сложные типы */
    ECO_VARIANT_1_COMPLEX64_T    = 0x28, /* 2 x float32 */
    ECO_VARIANT_1_COMPLEX128_T   = 0x29, /* 2 x float64 (ECO_DOUBLE_COMPLEX_T) */

    /* Квантованные типы (AI) */
    ECO_VARIANT_1_INT4_Q_T       = 0x30,
    ECO_VARIANT_1_UINT4_Q_T      = 0x31,

    /* Системные */
    ECO_VARIANT_1_BOOL_T         = 0x60,
    ECO_VARIANT_1_BIT_T          = 0x61,
    ECO_VARIANT_1_CHAR_T         = 0x62,  /* 1-байтовый ASCII символ */
    ECO_VARIANT_1_WCHAR_T        = 0x63,  /* Многобайтовый UTF-16 / UTF-32 символ */

    /* Указатели на текстовые массивы в String Arena */
    ECO_VARIANT_1_STR_T          = 0x64,  /* Классическая строка */
    ECO_VARIANT_1_WSTR_T         = 0x65,  /* Широкая строка */
    ECO_VARIANT_1_UTF8_T         = 0x66,  /* UTF-8 строка */
    ECO_VARIANT_1_UTF16_T        = 0x67,  /* UTF-16 строка */
    ECO_VARIANT_1_UTF32_T        = 0x68,  /* UTF-32 строка */
    
    /* Объекты и интерфейсы */
    ECO_VARIANT_1_OBJECT_T       = 0x69,  /* Обобщенный прикладной объект */
    ECO_VARIANT_1_UNKNOWN_T      = 0x6A,  /* Корень всех интерфейсов: честный IEcoUnknown* */
    ECO_VARIANT_1_INTERFACE_T    = 0x6B,  /* Специфичный интерфейс с известным IID */
    ECO_VARIANT_1_DISPATCH_T     = 0x6C,  /* Интерфейс позднего связывания IEcoDispatch* */
    
    /* Бизнес-математика и Системные маркеры */
    ECO_VARIANT_1_DECIMAL_T      = 0x6D,
    ECO_VARIANT_1_DATE_T         = 0x6E,
    ECO_VARIANT_1_CURRENCY_T     = 0x6F,
    ECO_VARIANT_1_ERROR_T        = 0x70,  /* Системный HRESULT / код ошибки */
    ECO_VARIANT_1_VARIANT_T      = 0x71,  /* Массив вариантов */
    ECO_VARIANT_1_PTR_T          = 0x72,  /* Сырой указатель void* */
    ECO_VARIANT_1_VOID_T         = 0x73,  /* Маркер отсутствия возвращаемого значения */
    ECO_VARIANT_1_RECORD_T       = 0x74,  /* Пользовательская Си-структура (UDT) */
    ECO_VARIANT_1_GUID_T         = 0x75,  /* GUID */
    ECO_VARIANT_1_UGUID_T        = 0x76,  /* UGUID */

    ECO_VARIANT_1_TENSOR_T       = 0x80,  /* Многомерный префиксный EcoTensor */
    
    /* Линейные контейнеры */
    ECO_VARIANT_1_LIST_1D_T      = 0x91,  /* Односвязный список */
    ECO_VARIANT_1_LIST_2D_T      = 0x92,  /* Двусвязный список */
    ECO_VARIANT_1_STACK_T        = 0x93,  /* Стек (LIFO) */
    ECO_VARIANT_1_QUEUE_T        = 0x94,  /* Очередь (FIFO) */
    
    /* Ассоциативные и Кэш-таблицы */
    ECO_VARIANT_1_SET_T          = 0xA0,  /* Множество (Tree Set) */
    ECO_VARIANT_1_MAP_T          = 0xA1,  /* Сбалансированная Мапа (Tree Map) */
    ECO_VARIANT_1_HASHMAP_T      = 0xA2,  /* Быстрая Хэш-таблица */
    ECO_VARIANT_1_DICTIONARY_T   = 0xA3,  /* Словарь (Префиксное дерево / Trie) */

    /* Пространственные и Структурные деревья */
    ECO_VARIANT_1_BIN_TREE_T     = 0xAD,  /* Несбалансированное бинарное дерево */
    ECO_VARIANT_1_BS_TREE_T      = 0xAE,  /* Несбалансированное бинарное дерево поиска (BST) */
    ECO_VARIANT_1_RB_TREE_T      = 0xAF,  /* Сбалансированное красно-черное дерево поиска */
    ECO_VARIANT_1_R_TREE_T       = 0xB0,  /* Пространственный R-Tree индекс */
    ECO_VARIANT_1_B_TREE_T       = 0xB1,  /* Сильноветвистое B-Tree / B+Tree */
    ECO_VARIANT_1_QUAD_OCT_TREE_T= 0xB2,  /* Графические QuadTree / Octree */
    ECO_VARIANT_1_KD_TREE_T      = 0xB3,  /* k-d Tree для AI/KNN поиска */

    /* Графы и Вероятностные структуры Big Data */
    ECO_VARIANT_1_GRAPH_T        = 0xB4,  /* Сетевой / Математический взвешенный Граф */
    ECO_VARIANT_1_BLOOM_FILTER_T = 0xB5,  /* Вероятностный Фильтр Блума */
    ECO_VARIANT_1_HYPERLOGLOG_T  = 0xB6,  /* Вероятностный счетчик HyperLogLog */
    ECO_VARIANT_1_MERKLE_TREE_T  = 0xB7,  /* Криптографическое Дерево Меркла */

    /* Системные структуры реального времени (RTOS) */
    ECO_VARIANT_1_PRIORITY_QUEUE_T=0xC0,  /* Очередь с приоритетом (Куча / Heap) */
    ECO_VARIANT_1_RING_BUFFER_T  = 0xC1,  /* Аппаратный / Системный Кольцевой буфер */
    ECO_VARIANT_1_SKIP_LIST_T    = 0xC2,  /* Многопоточный Lock-Free Список с пропусками */
    ECO_VARIANT_1_CUSTOM_T       = 0xC3,

    /*   БИТОВЫЕ МОДИФИКАТОРЫ (Занимают старшие биты 16-битного поля type)   */
    ECO_VARIANT_1_ARRAY          = 0x2000, /* Флаг: Данные упакованы в EcoTensor */
    ECO_VARIANT_1_BYREF          = 0x4000, /* Флаг: Указатель на тип (void* / Передача по ссылке) */
    ECO_VARIANT_1_PREFIX         = 0x8000  /* Флаг: Данные снабжены интрузивными метаданными */

} ECO_VARIANT_1_TYPE;

#pragma pack(push, 1)
typedef struct ECO_VARIANT_V1_T {
    uint16_t type;     /* Тип данных (наше единое перечисление ECO_VARIANT_1_TYPE) */
    uint8_t  meta;     /* Битовые флаги для деревьев (цвет, баланс) */
    uint8_t  reserved;
    uint32_t id;       /* ID ячейки */
    
    union {
        /* ===================================================================== */
        /* 1. ССЫЛОЧНЫЕ И СИСТЕМНЫЕ ТИПЫ (64-битные адреса)                      */
        /* ===================================================================== */
        void*               ptr;       /* Универсальный сырой указатель (void*) */
        const char*         cstr;      /* Указатель на ASCII / UTF-8 строки */
        struct IEcoUnknown* unk;       /* Корень интерфейсной модели Eco */
        
        /* ===================================================================== */
        /* 2. ЦЕЛОЧИСЛЕННЫЙ КРОССПЛАТФОРМЕННЫЙ МАППИНГ (Little-Endian)           */
        /* ===================================================================== */
        struct {
            union {
                uint64_t q;     /* Quadword: 64 бита (Аналог RAX / x0 / s0) */
                int64_t  q_s;   /* Знаковый аналог для INT64 */
                
                struct {
                    union {
                        uint32_t d;     /* Doubleword: младшие 32 бита (Аналог EAX / w0) */
                        int32_t  d_s;   /* Знаковый аналог для INT32 */
                        
                        struct {
                            union {
                                uint16_t w;    /* Word: младшие 16 бит (Аналог AX) */
                                int16_t  w_s;  /* Знаковый аналог для INT16 */
                                
                                struct {
                                    uint8_t b0; /* Младший байт 0 (Аналог AL, для BOOL/INT8) */
                                    uint8_t b1; /* Байт 1 (Аналог AH) */
                                };
                            };
                            uint16_t w_hi;     /* Старшие 16 бит от младшего 32-битного слова */
                        };
                    };
                    uint32_t d_hi;      /* Старшие 32 бита 64-битного регистра (Ваш sub.hi) */
                    int32_t  d_hi_s;    /* Знаковый старший doubleword */
                };
            };
        } u;

        /* ===================================================================== */
        /* 3. КРОССПЛАТФОРМЕННЫЙ МАППИНГ ПЛАВАЮЩЕЙ ТОЧКИ (xmm/ymm, v0-v31, f0-f31)*/
        /* ===================================================================== */
        struct {
            union {
                double f64;     /* 64-битный Float (Аналог xmm0[64], d0 в ARM, f0 в RISC-V) */
                
                struct {
                    float f32;    /* Младший 32-битный Float (Аналог xmm0[32], s0 в ARM) */
                    float f32_hi; /* Старший 32-битный Float (Для ECO_VARIANT_1_COMPLEX64_T) */
                };
            };
        } f;

        /* ===================================================================== */
        /* 4. СВЕРХКОМПАКТНЫЕ И КВАНТОВАННЫЕ ВЕКТОРЫ AI                          */
        /* ===================================================================== */
        int16_t  f16_pack[4];   /* 4 x FP16 / bfloat16 (Заполняют все 64 бита) */
        int8_t   i8_pack[8];    /* 8 x INT8 квантованных весов */

    } value;

} ECO_VARIANT_V1_T;
#pragma pack(pop)
#define ECO_VARIANT_V1_T_DEFINED

#endif

#ifdef UGUID_UTILITY

#define IsEqualUGUID(rguid1, rguid2)  \
    ((((byte_t *) rguid1)[0] == ((byte_t *) rguid2)[0]) &&   \
    (((byte_t *) rguid1)[1] == ((byte_t *) rguid2)[1]) &&    \
    (((byte_t *) rguid1)[2] == ((byte_t *) rguid2)[2]) &&    \
    (((byte_t *) rguid1)[3] == ((byte_t *) rguid2)[3]) &&    \
    (((byte_t *) rguid1)[4] == ((byte_t *) rguid2)[4]) &&    \
    (((byte_t *) rguid1)[5] == ((byte_t *) rguid2)[5]) &&    \
    (((byte_t *) rguid1)[6] == ((byte_t *) rguid2)[6]) &&    \
    (((byte_t *) rguid1)[7] == ((byte_t *) rguid2)[7]) &&    \
    (((byte_t *) rguid1)[8] == ((byte_t *) rguid2)[8]) &&    \
    (((byte_t *) rguid1)[9] == ((byte_t *) rguid2)[9]) &&    \
    (((byte_t *) rguid1)[10] == ((byte_t *) rguid2)[10]) &&    \
    (((byte_t *) rguid1)[11] == ((byte_t *) rguid2)[11]) &&    \
    (((byte_t *) rguid1)[12] == ((byte_t *) rguid2)[12]) &&    \
    (((byte_t *) rguid1)[13] == ((byte_t *) rguid2)[13]) &&    \
    (((byte_t *) rguid1)[14] == ((byte_t *) rguid2)[14]) &&    \
    (((byte_t *) rguid1)[15] == ((byte_t *) rguid2)[15]) &&    \
    (((byte_t *) rguid1)[16] == ((byte_t *) rguid2)[16]) &&    \
    (((byte_t *) rguid1)[17] == ((byte_t *) rguid2)[17]))

#endif

/* IEcoUnknown IID = 00000000-0000-0000-0000-0000000000AA */
#ifndef __IID_IEcoUnknown
static const UGUID IID_IEcoUnknown = { 0x01, 0x10, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA} };
#endif /* __IID_IEcoUnknown */

#ifndef ECO_I_ECO_UNKNOWN_PTR_T_DEFINED
typedef struct IEcoUnknown* IEcoUnknownPtr_t;
#define ECO_I_ECO_UNKNOWN_PTR_T_DEFINED
#endif

typedef struct IEcoUnknownVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoUnknownPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoUnknownPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoUnknownPtr_t me);

} IEcoUnknownVTbl;

interface IEcoUnknown {
    struct IEcoUnknownVTbl* pVTbl;
} IEcoUnknown;

/* IEcoComponentFactory IID = 00000000-0000-0000-0000-000000000055 */
#ifndef __IID_IEcoComponentFactory
static const UGUID IID_IEcoComponentFactory = { 0x01, 0x10, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55} };
#endif /* __IID_IEcoComponentFactory */

#ifndef ECO_I_ECO_COMPONENT_FACTORY_PTR_T_DEFINED
typedef struct IEcoComponentFactory* IEcoComponentFactoryPtr_t;
#define ECO_I_ECO_COMPONENT_FACTORY_PTR_T_DEFINED
#endif

typedef struct IEcoComponentFactoryVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoComponentFactoryPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoComponentFactoryPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoComponentFactoryPtr_t me);

    int16_t (ECOCALLMETHOD *Alloc)(/* in */ IEcoComponentFactoryPtr_t me, /* in */ IEcoUnknownPtr_t pISystem, /* in */ IEcoUnknownPtr_t pIUnknownOuter, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    int16_t (ECOCALLMETHOD *Init)(/* in */ IEcoComponentFactoryPtr_t me, /* in */ IEcoUnknownPtr_t pISystem, /* in */ voidptr_t pv);
    char_t* (ECOCALLMETHOD *get_Name)(/* in */ IEcoComponentFactoryPtr_t me);
    char_t* (ECOCALLMETHOD *get_Version)(/* in */ IEcoComponentFactoryPtr_t me);
    char_t* (ECOCALLMETHOD *get_Manufacturer)(/* in */ IEcoComponentFactoryPtr_t me);

} IEcoComponentFactoryVTbl;

interface IEcoComponentFactory {
    struct IEcoComponentFactoryVTbl* pVTbl;
} IEcoComponentFactory;

/* Определение функции и структуры создания экземпляра для фабрики компонентов */
typedef int16_t (ECOCALLMETHOD *CreateInstance)(voidptr_t, voidptr_t, voidptr_t*);

/* Определение функции инициализации экземпляра для фабрики компонентов */
typedef int16_t (ECOCALLMETHOD *InitInstance)(voidptr_t, voidptr_t);

#endif /* __I_ECO_BASE_1_H__ */
