/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#include "polyfit_bus.h"

/* Resolve the normal-equations implementation from the interface bus. */
int16_t PolyfitComponent_QueryFromBus(IEcoInterfaceBus1Ptr_t bus,
    IEcoPolynomialMathPtr* component) {
    if (component == 0) return ERR_ECO_POINTER;
    *component = 0;
    if (bus == 0 || bus->pVTbl == 0) return ERR_ECO_POINTER;
    return bus->pVTbl->QueryComponent(bus, &CID_EcoPolynomialMath, 0,
        &IID_IEcoPolynomialMath, (voidptr_t*)component);
}

/* In the static configuration, register a complete graph in one transaction.
 * No component is constructed until QueryComponent is called. */
#if !defined(ECO_CALCULATOR_KIND) && !defined(ECO_LAB2_ONLY)
typedef struct FactoryEntry {
    const UGUID* cid;
    IEcoComponentFactory* (*factory)(void);
} FactoryEntry;

static const FactoryEntry entries[] = {
    {&CID_EcoPolynomialMath, GetPolynomialFactory_00000045},
    {&CID_EcoPolynomialMathQR, GetPolynomialQRFactory_00000046},
    {&CID_EcoCalculatorB, GetCalculatorBFactory},
    {&CID_EcoCalculatorE, GetCalculatorEFactory},
#ifdef ECO_REPLACE_A_WITH_C
    {&CID_EcoCalculatorA, GetCalculatorCFactory},
#else
    {&CID_EcoCalculatorA, GetCalculatorAFactory},
#endif
    {&CID_EcoCalculatorD, GetCalculatorDFactory}
#if defined(ECO_LIB) || defined(ECO_DLL)
    ,{&CID_EcoLab2, GetEcoLab2Factory}
#endif
};

static int16_t register_entries(IEcoInterfaceBus1* bus, const FactoryEntry* list, uint32_t count) {
    uint32_t i;
    int16_t status;
    if (!bus || !bus->pVTbl) return ERR_ECO_POINTER;
    for (i = 0; i < count; ++i) {
        status = bus->pVTbl->RegisterComponent(bus, list[i].cid, (IEcoUnknown*)list[i].factory());
        if (status != ERR_ECO_SUCCESS) {
            while (i) bus->pVTbl->UnRegisterComponent(bus, list[--i].cid);
            return status;
        }
    }
    return ERR_ECO_SUCCESS;
}
int16_t PolyfitComponent_RegisterOnBus(IEcoInterfaceBus1* bus) {
    return register_entries(bus, entries, 4); /* normal, QR, B and E */
}
int16_t CalculatorComponents_RegisterOnBus(IEcoInterfaceBus1* bus) {
    return register_entries(bus, entries + 2, 4); /* B, E, A/C and D */
}
#if defined(ECO_LIB) || defined(ECO_DLL)
int16_t EcoLab2_RegisterOnBus(IEcoInterfaceBus1* bus) {
    return register_entries(bus, entries, sizeof(entries)/sizeof(entries[0]));
}
#endif
#endif
