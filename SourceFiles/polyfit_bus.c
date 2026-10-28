/*
 * Кодировка: UTF-8 with signature (Codepage 65001).
 * Назначение: исходный файл компонента лабораторной работы №1, задание 44.
 * Описание: ACOM/Eco-компонент полиномиальной аппроксимации.
 * Автор: Анвари Д.А., 24ТИДИ.
 */

#include "polyfit_bus.h"

/* Register both task-44 implementations on the supplied interface bus. */
int16_t PolyfitComponent_RegisterOnBus(IEcoInterfaceBus1Ptr_t bus) {
    int16_t status;
    if (bus == 0 || bus->pVTbl == 0) return ERR_ECO_POINTER;
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoPolynomialMath,
        (IEcoUnknownPtr_t)GetPolynomialFactory_00000045());
    if (status != ERR_ECO_SUCCESS) return status;
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoPolynomialMathQR,
        (IEcoUnknownPtr_t)GetPolynomialQRFactory_00000046());
    if (status != ERR_ECO_SUCCESS)
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMath);
    return status;
}

/* Resolve the normal-equations implementation from the interface bus. */
int16_t PolyfitComponent_QueryFromBus(IEcoInterfaceBus1Ptr_t bus,
    IEcoPolynomialMathPtr* component) {
    if (component == 0) return ERR_ECO_POINTER;
    *component = 0;
    if (bus == 0 || bus->pVTbl == 0) return ERR_ECO_POINTER;
    return bus->pVTbl->QueryComponent(bus, &CID_EcoPolynomialMath, 0,
        &IID_IEcoPolynomialMath, (voidptr_t*)component);
}

#if !defined(ECO_CALCULATOR_KIND) && !defined(ECO_LAB2_ONLY)
#include "IdEcoLab2.h"
int16_t CalculatorComponents_RegisterOnBus(IEcoInterfaceBus1* bus) {
    const UGUID* cids[4]; IEcoComponentFactory* factories[4];
    uint32_t i; int16_t status;
    if (bus == 0) return ERR_ECO_POINTER;
    cids[0] = &CID_EcoCalculatorA; cids[1] = &CID_EcoCalculatorB;
    cids[2] = &CID_EcoCalculatorD; cids[3] = &CID_EcoCalculatorE;
#ifdef ECO_REPLACE_A_WITH_C
    factories[0] = GetCalculatorCFactory();
#else
    factories[0] = GetCalculatorAFactory();
#endif
    factories[1] = GetCalculatorBFactory(); factories[2] = GetCalculatorDFactory();
    factories[3] = GetCalculatorEFactory();
    for (i = 0u; i < 4u; ++i) {
        status = bus->pVTbl->RegisterComponent(bus, cids[i], (IEcoUnknown*)factories[i]);
        if (status != ERR_ECO_SUCCESS) {
            while (i > 0u) { --i; bus->pVTbl->UnRegisterComponent(bus, cids[i]); }
            return status;
        }
    }
    return ERR_ECO_SUCCESS;
}

#endif

#if !defined(ECO_CALCULATOR_KIND) && !defined(ECO_LAB2_ONLY)
#if defined(ECO_LIB) || defined(ECO_DLL)
int16_t EcoLab2_RegisterOnBus(IEcoInterfaceBus1Ptr_t bus) {
    int16_t status;
    if (bus == 0) return ERR_ECO_POINTER;
    status = PolyfitComponent_RegisterOnBus(bus);
    if (status != ERR_ECO_SUCCESS) return status;
    status = CalculatorComponents_RegisterOnBus(bus);
    if (status != ERR_ECO_SUCCESS) {
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMathQR);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMath);
        return status;
    }
    status = bus->pVTbl->RegisterComponent(bus, &CID_EcoLab2,
        (IEcoUnknownPtr_t)GetEcoLab2Factory());
    if (status != ERR_ECO_SUCCESS) {
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorE);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorD);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorB);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoCalculatorA);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMathQR);
        bus->pVTbl->UnRegisterComponent(bus, &CID_EcoPolynomialMath);
    }
    return status;
}
#endif

#endif
