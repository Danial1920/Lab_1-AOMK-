/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   IEcoInterfaceBus1Bridge
 * </сводка>
 *
 * <описание>
 *   Данный заголовок описывает реализацию интерфейсов IEcoInterfaceBus1Bridge
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#ifndef __I_ECO_INTERFACE_BUS_1_BRIDGE_H__
#define __I_ECO_INTERFACE_BUS_1_BRIDGE_H__

#include "IEcoBase1.h"

/* IEcoInterfaceBus1Bridge IID = {00000000-0000-0000-0000-A00400000101} */
#ifndef __IID_IEcoInterfaceBus1Bridge
static const UGUID IID_IEcoInterfaceBus1Bridge = { 0x01, 0x10, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x05, 0x00, 0x00, 0x01, 0x01} };
#endif /* __IID_IEcoInterfaceBus1Bridge */

typedef struct IEcoInterfaceBus1Bridge* IEcoInterfaceBus1BridgePtr_t;

typedef struct IEcoInterfaceBus1BridgeVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface )(/* in */ IEcoInterfaceBus1BridgePtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef )(/* in */ IEcoInterfaceBus1BridgePtr_t me);
    uint32_t (ECOCALLMETHOD *Release )(/* in */ IEcoInterfaceBus1BridgePtr_t me);

    int16_t (ECOCALLMETHOD *QueryComponent) (/*in*/ IEcoInterfaceBus1BridgePtr_t me, /*in*/ const UGUID* rcid, /*in*/ IEcoUnknownPtr_t pIUnkOuter, /*in*/ const UGUID* riid, /*out*/ voidptr_t* ppv);

} IEcoInterfaceBus1BridgeVTbl, *IEcoInterfaceBus1BridgeVTblPtr;

interface IEcoInterfaceBus1Bridge {
    struct IEcoInterfaceBus1BridgeVTbl *pVTbl;
} IEcoInterfaceBus1Bridge;

#endif /* __I_ECO_INTERFACE_BUS_1_BRIDGE_H__ */
