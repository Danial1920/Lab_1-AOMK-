/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   depend.h
 * </сводка>
 *
 * <описание>
 *   Данный заголовок подключаеет зависимые от архитектуры и платформы заголовочные файлы
 *   ПРИМЕЧАНИЕ: Данный файл расширяемый и может менятся в зависимости от добавляемой поддержки той или иной архитектуры и платформы.
 *               
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2016 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#ifndef __ECO_DEPEND_H__
#define __ECO_DEPEND_H__

/* Подключение заголовочных файлов для Эко ОС */
#ifdef ECO_OS

/* STD ANSI C 1998 */

#ifdef ECO_BCM283X 
#include <sys/types.h>
#define ECO_INT8_T_DEFINED
#define ECO_UINT8_T_DEFINED
#define ECO_INT16_T_DEFINED
#define ECO_UINT16_T_DEFINED
#define ECO_INT32_T_DEFINED
#define ECO_UINT32_T_DEFINED
#define ECO_INT64_T_DEFINED
#define ECO_UINT64_T_DEFINED
#define ECO_WCHAR_T_DEFINED
#endif

/* MISRA C 1998 */
#ifndef ECO_INT8_T_DEFINED
typedef char int8_t;
#define ECO_INT8_T_DEFINED
#endif

#ifndef ECO_UINT8_T_DEFINED
typedef unsigned char uint8_t;
#define ECO_UINT8_T_DEFINED
#endif

#ifndef ECO_INT16_T_DEFINED
typedef short int16_t;
#define ECO_INT16_T_DEFINED
#endif

#ifndef ECO_UINT16_T_DEFINED
typedef unsigned short uint16_t;
#define ECO_UINT16_T_DEFINED
#endif

#ifndef ECO_WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define ECO_WCHAR_T_DEFINED
#endif

#ifndef ECO_INT32_T_DEFINED
typedef long int32_t;
#define ECO_INT32_T_DEFINED
#endif

#ifndef ECO_UINT32_T_DEFINED
typedef unsigned long uint32_t;
#define ECO_UINT32_T_DEFINED
#endif

#ifndef ECO_INT64_T_DEFINED
typedef long long int int64_t;
#define ECO_INT64_T_DEFINED
#endif

#ifndef ECO_UINT64_T_DEFINED
typedef unsigned long long int uint64_t;
#define ECO_UINT64_T_DEFINED
#endif

#ifndef ECO_LDOUBLE_T_DEFINED
typedef long double ldouble_t;
#define ECO_LDOUBLE_T_DEFINED
#endif

#ifndef ECO_BOOL_T_DEFINED
typedef unsigned char bool_t;
#define ECO_BOOL_T_DEFINED
#endif

#ifndef ECO_BYTE_T_DEFINED
typedef unsigned char byte_t;
#define ECO_BYTE_T_DEFINED
#endif

#ifndef ECO_CHAR_T_DEFINED
typedef char char_t;
#define ECO_CHAR_T_DEFINED
#endif

#ifndef ECO_UCHAR_T_DEFINED
typedef unsigned char uchar_t;
#define ECO_UCHAR_T_DEFINED
#endif

#ifndef ECO_FLOAT_T_DEFINED
typedef float float_t;
#define ECO_FLOAT_T_DEFINED
#endif

#ifndef ECO_DOUBLE_T_DEFINED
typedef double double_t;
#define ECO_DOUBLE_T_DEFINED
#endif

#ifndef ECO_LONG_T_DEFINED
typedef long long_t;
#define ECO_LONG_T_DEFINED
#endif

#ifndef ECO_ULONG_T_DEFINED
typedef unsigned long ulong_t;
#define ECO_ULONG_T_DEFINED
#endif

#ifndef ECO_VOID_PTR_T_DEFINED
typedef void* voidptr_t;
#define ECO_VOID_PTR_T_DEFINED
#endif

#ifndef ECO_DESCRIPTOR_T_DEFINED
typedef void* descriptor_t;
#define ECO_DESCRIPTOR_T_DEFINED
#endif

#ifndef ECO_HANDLE_DEFINED
typedef void* handle_t;
#define ECO_HANDLE_DEFINED
#define ECO_INVALID_HANDLE ((handle_t)0)
#endif

/* Подключение заголовочных файлов для ОС Windows */
#elif ECO_WINDOWS

#define ECO_HUGE_VAL_DEFINED
#define ECO_DIV_T_DEFINED

/* STD ANSI C 1998 */
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* MISRA C 1998 */
#include <stdint.h>

#ifndef ECO_BOOL_T_DEFINED
typedef unsigned char bool_t;
#define ECO_BOOL_T_DEFINED
#endif

#ifndef ECO_BYTE_T_DEFINED
typedef unsigned char byte_t;
#define ECO_BYTE_T_DEFINED
#endif

#ifndef ECO_CHAR_T_DEFINED
typedef char char_t;
#define ECO_CHAR_T_DEFINED
#endif

#ifndef ECO_UCHAR_T_DEFINED
typedef unsigned char uchar_t;
#define ECO_UCHAR_T_DEFINED
#endif

#ifndef ECO_FLOAT_T_DEFINED
typedef float float_t;
#define ECO_FLOAT_T_DEFINED
#endif

#ifndef ECO_DOUBLE_T_DEFINED
typedef double double_t;
#define ECO_DOUBLE_T_DEFINED
#endif

#ifndef ECO_LONG_T_DEFINED
typedef long long_t;
#define ECO_LONG_T_DEFINED
#endif

#ifndef ECO_ULONG_T_DEFINED
typedef unsigned long ulong_t;
#define ECO_ULONG_T_DEFINED
#endif

#ifndef ECO_VOID_PTR_T_DEFINED
typedef void* voidptr_t;
#define ECO_VOID_PTR_T_DEFINED
#endif

#ifndef ECO_DESCRIPTOR_T_DEFINED
typedef void* descriptor_t;
#define ECO_DESCRIPTOR_T_DEFINED
#endif

#ifndef ECO_HANDLE_DEFINED
typedef void* handle_t;
#define ECO_HANDLE_DEFINED
#define ECO_INVALID_HANDLE ((handle_t)0)
#endif

/* Windows OS optional */
#ifdef ECO_WINDOWS_OPT
#include "windows.h"
#include <winioctl.h>
#include "dshow.h"
#pragma comment(lib, "strmiids.lib")
#include <CRTDBG.H>
#endif

/* Подключение заголовочных файлов для ОС Linux */
#elif ECO_LINUX

/* STD ANSI C 1998 */
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* MISRA C 1998 */
#include <stdint.h>

/* Linux OS optional */
#ifdef ECO_LINUX_OPT
#include <dlfcn.h>
#include <err.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>
#endif

#ifndef ECO_BOOL_T_DEFINED
typedef unsigned char bool_t;
#define ECO_BOOL_T_DEFINED
#endif

#ifndef ECO_BYTE_T_DEFINED
typedef unsigned char byte_t;
#define ECO_BYTE_T_DEFINED
#endif

#ifndef ECO_CHAR_T_DEFINED
typedef char char_t;
#define ECO_CHAR_T_DEFINED
#endif

#ifndef ECO_UCHAR_T_DEFINED
typedef unsigned char uchar_t;
#define ECO_UCHAR_T_DEFINED
#endif

#ifndef ECO_FLOAT_T_DEFINED
typedef float float_t;
#define ECO_FLOAT_T_DEFINED
#endif

#ifndef ECO_DOUBLE_T_DEFINED
typedef double double_t;
#define ECO_DOUBLE_T_DEFINED
#endif

#ifndef ECO_LONG_T_DEFINED
typedef long long_t;
#define ECO_LONG_T_DEFINED
#endif

#ifndef ECO_ULONG_T_DEFINED
typedef unsigned long ulong_t;
#define ECO_ULONG_T_DEFINED
#endif

#ifndef ECO_VOID_PTR_T_DEFINED
typedef void* voidptr_t;
#define ECO_VOID_PTR_T_DEFINED
#endif

#ifndef ECO_DESCRIPTOR_T_DEFINED
typedef void* descriptor_t;
#define ECO_DESCRIPTOR_T_DEFINED
#endif

#ifndef ECO_HANDLE_DEFINED
typedef void* handle_t;
#define ECO_HANDLE_DEFINED
#define ECO_INVALID_HANDLE ((handle_t)0)
#endif

#define ECO_DIV_T_DEFINED
#define ECO_SIZE_T_DEFINED
#define ECO_WCHAR_T_DEFINED


/* Подключение заголовочных файлов для ОС Mac OS X, iOS, tvOS */
#elif ECO_APPLE

/* STD ANSI C 1998 */
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* MISRA C 1998 */
#include <stdint.h>

/* Mac OS X/iOS optional */

#ifndef ECO_BOOL_T_DEFINED
typedef unsigned char bool_t;
#define ECO_BOOL_T_DEFINED
#endif

#ifndef ECO_BYTE_T_DEFINED
typedef unsigned char byte_t;
#define ECO_BYTE_T_DEFINED
#endif

#ifndef ECO_CHAR_T_DEFINED
typedef char char_t;
#define ECO_CHAR_T_DEFINED
#endif

#ifndef ECO_UCHAR_T_DEFINED
typedef unsigned char uchar_t;
#define ECO_UCHAR_T_DEFINED
#endif

#ifndef ECO_FLOAT_T_DEFINED
typedef float float_t;
#define ECO_FLOAT_T_DEFINED
#endif

#ifndef ECO_DOUBLE_T_DEFINED
typedef double double_t;
#define ECO_DOUBLE_T_DEFINED
#endif

#ifndef ECO_LONG_T_DEFINED
typedef long long_t;
#define ECO_LONG_T_DEFINED
#endif

#ifndef ECO_ULONG_T_DEFINED
typedef unsigned long ulong_t;
#define ECO_ULONG_T_DEFINED
#endif

#ifndef ECO_VOID_PTR_T_DEFINED
typedef void* voidptr_t;
#define ECO_VOID_PTR_T_DEFINED
#endif

#ifndef ECO_DESCRIPTOR_T_DEFINED
typedef void* descriptor_t;
#define ECO_DESCRIPTOR_T_DEFINED
#endif

#ifndef ECO_HANDLE_DEFINED
typedef void* handle_t;
#define ECO_HANDLE_DEFINED
#define ECO_INVALID_HANDLE ((handle_t)0)
#endif

/* Подключение заголовочных файлов для ОС Android */
#elif ECO_ANDROID
/* STD ANSI C 1998 */
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* MISRA C 1998 */
#include <stdint.h>

/* Andeoid OS optional */

#ifndef ECO_BOOL_T_DEFINED
typedef unsigned char bool_t;
#define ECO_BOOL_T_DEFINED
#endif

#ifndef ECO_BYTE_T_DEFINED
typedef unsigned char byte_t;
#define ECO_BYTE_T_DEFINED
#endif

#ifndef ECO_CHAR_T_DEFINED
typedef char char_t;
#define ECO_CHAR_T_DEFINED
#endif

#ifndef ECO_UCHAR_T_DEFINED
typedef unsigned char uchar_t;
#define ECO_UCHAR_T_DEFINED
#endif

#ifndef ECO_FLOAT_T_DEFINED
typedef float float_t;
#define ECO_FLOAT_T_DEFINED
#endif

#ifndef ECO_DOUBLE_T_DEFINED
typedef double double_t;
#define ECO_DOUBLE_T_DEFINED
#endif

#ifndef ECO_LONG_T_DEFINED
typedef long long_t;
#define ECO_LONG_T_DEFINED
#endif

#ifndef ECO_ULONG_T_DEFINED
typedef unsigned long ulong_t;
#define ECO_ULONG_T_DEFINED
#endif

#ifndef ECO_VOID_PTR_T_DEFINED
typedef void* voidptr_t;
#define ECO_VOID_PTR_T_DEFINED
#endif

#ifndef ECO_DESCRIPTOR_T_DEFINED
typedef void* descriptor_t;
#define ECO_DESCRIPTOR_T_DEFINED
#endif

#ifndef ECO_HANDLE_DEFINED
typedef void* handle_t;
#define ECO_HANDLE_DEFINED
#define ECO_INVALID_HANDLE ((handle_t)0)
#endif

#else
#error Please define the platform or architecture
#endif

#ifdef ECO_WINDOWS

#ifdef _MSC_VER
#define ECO_EXPORT __declspec(dllexport)
#define ECOCDECLMETHOD __cdecl
#define ECOCALLMETHOD __stdcall
#define ECOSTDCALLMETHOD __stdcall
#define ECOFASTCALLMETHOD __fastcall
#define ECOPASCALMETHOD
#define ECOSAFECALLMETHOD
#define ECOTHISCALLMETHOD
#define ECOVECTORCALLMETHOD
#define ECO_WEAK __declspec(selectany)
#else
#define ECO_EXPORT
#define ECOCDECLMETHOD __attribute__((cdecl))
#define ECOCALLMETHOD __attribute__((stdcall))
#define ECOSTDCALLMETHOD __attribute__((stdcall))
#define ECOFASTCALLMETHOD __attribute__((fastcall))
#define ECOPASCALMETHOD
#define ECOSAFECALLMETHOD
#define ECOTHISCALLMETHOD
#define ECOVECTORCALLMETHOD
#define ECO_WEAK __attribute__((weak))
#endif
#else
#define ECO_EXPORT
#define ECOCDECLMETHOD
#define ECOCALLMETHOD
#define ECOSTDCALLMETHOD
#define ECOFASTCALLMETHOD
#define ECOPASCALMETHOD
#define ECOSAFECALLMETHOD
#define ECOTHISCALLMETHOD
#define ECOVECTORCALLMETHOD
#define ECO_WEAK
#endif

/* Порядок байтов */
#include "byteswap.h"
/* Атомарные операции */
#include "atomic.h"

#endif /* __ECO_DEPEND_H__ */
