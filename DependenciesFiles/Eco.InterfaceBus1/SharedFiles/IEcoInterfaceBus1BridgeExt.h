/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   IEcoInterfaceBus1BridgeExt
 * </сводка>
 *
 * <описание>
 *   Данный заголовок описывает реализацию интерфейсов IEcoInterfaceBus1BridgeExt
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#ifndef __I_ECO_INTERFACE_BUS_1_BRIDGE_EXTENSION_H__
#define __I_ECO_INTERFACE_BUS_1_BRIDGE_EXTENSION_H__

#include "IEcoBase1.h"

/* IEcoInterfaceBus1BridgeExt IID = {00000000-0000-0000-0000-A00400000101} */
#ifndef __IID_IEcoInterfaceBus1BridgeExt
static const UGUID IID_IEcoInterfaceBus1BridgeExt = { 0x01, 0x10, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x04, 0x00, 0x00, 0x01, 0x01} };
#endif /* __IID_IEcoInterfaceBus1BridgeExt */

typedef struct IEcoInterfaceBus1BridgeExt* IEcoInterfaceBus1BridgeExtPtr_t;

typedef struct IEcoInterfaceBus1BridgeExtVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface )(/* in */ IEcoInterfaceBus1BridgeExtPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef )(/* in */ IEcoInterfaceBus1BridgeExtPtr_t me);
    uint32_t (ECOCALLMETHOD *Release )(/* in */ IEcoInterfaceBus1BridgeExtPtr_t me);

    /* <метод>
     *   RegisterBridge
     * </метод>
     * <описание>
     *   Установка поддерживаемого шиной расширения интерфейса дополнительного компонента управления памятью
     * </описание>
     */
    int16_t (ECOCALLMETHOD *RegisterBridge) (/*in*/ IEcoInterfaceBus1BridgeExtPtr_t me, /* in */ const UGUID* rcid, /* in */ char_t* name);

    /* <метод>
     *   get_Manager
     * </метод>
     * <описание>
     *   Возвращает идентификатор компонента управления памятью
     * </описание>
     */
    int16_t (ECOCALLMETHOD *UnRegisterBridge) (/*in*/ IEcoInterfaceBus1BridgeExtPtr_t me, /* in */ const UGUID* rcid);

} IEcoInterfaceBus1BridgeExtVTbl, *IEcoInterfaceBus1BridgeExtVTblPtr;

interface IEcoInterfaceBus1BridgeExt {
    struct IEcoInterfaceBus1BridgeExtVTbl *pVTbl;
} IEcoInterfaceBus1BridgeExt;

#endif /* __I_ECO_INTERFACE_BUS_1_BRIDGE_EXTENSION_H__ */
