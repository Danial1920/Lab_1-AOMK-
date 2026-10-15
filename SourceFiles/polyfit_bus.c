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
