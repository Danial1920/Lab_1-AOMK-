#ifndef ECO_LAB_CONNECTION_INTERFACES_H
#define ECO_LAB_CONNECTION_INTERFACES_H
/* The C89 SDK forward-declares these two pointer typedefs twice. */
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtypedef-redefinition"
#endif
#include "IEcoConnectionPointContainer.h"
#include "IEcoEnumConnections.h"
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif
