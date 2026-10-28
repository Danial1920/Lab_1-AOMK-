#include <stdio.h>
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "/tmp/eco-zzz-review-20261028/Eco.CalculatorA/SharedFiles/IEcoCalculatorX.h"
#include "/tmp/eco-zzz-review-20261028/Eco.CalculatorC/SharedFiles/IEcoCalculatorY.h"
extern int16_t EcoLab2_RegisterOnBus(IEcoInterfaceBus1* bus);
#define OUR_CID(n) {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,n}}
static const UGUID ids[] = {OUR_CID(0x60),OUR_CID(0x61),OUR_CID(0x62),OUR_CID(0x63),OUR_CID(0x64),OUR_CID(0x45),OUR_CID(0x46),OUR_CID(0x65)};
static const char* names[] = {"A","B","C","D","E","Lab1","Lab1QR","Lab2"};
int16_t EcoMain(IEcoUnknown* entry) {
    IEcoSystem1* system = 0;
    IEcoInterfaceBus1* bus = 0;
    IEcoUnknown* root = 0;
    IEcoCalculatorX* x = 0;
    IEcoCalculatorY* y = 0;
    int16_t status;
    unsigned i, failed = 0;
    if (entry == 0) return -1;
    status = entry->pVTbl->QueryInterface(entry, &GID_IEcoSystem, (void**)&system);
    if (status != 0 || system == 0) return -1;
    status = system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1, (void**)&bus);
    if (status != 0 || bus == 0) { system->pVTbl->Release(system); return -1; }
    status = EcoLab2_RegisterOnBus(bus);
    printf("Production registration helper: %d\n", (int)status);
    if (status != 0) { bus->pVTbl->Release(bus); system->pVTbl->Release(system); return -1; }
    for (i = 0; i < 8; ++i) {
        root = 0;
        status = bus->pVTbl->QueryComponent(bus, &ids[i], 0, &IID_IEcoUnknown, (void**)&root);
        if (status != 0 || root == 0) { ++failed; continue; }
        x = 0;
        status = root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorX, (void**)&x);
        printf("%s official X: status=%d pointer=%p\n", names[i], (int)status, (void*)x);
        if (status != 0 || x == 0) ++failed;
        else { if (x->pVTbl->Addition(x, 3, 5) != 8) ++failed; x->pVTbl->Release(x); }
        if (i >= 2) {
            y = 0;
            status = root->pVTbl->QueryInterface(root, &IID_IEcoCalculatorY, (void**)&y);
            printf("%s official Y: status=%d pointer=%p\n", names[i], (int)status, (void*)y);
            if (status != 0 || y == 0) ++failed;
            else { if (y->pVTbl->Division(y, 20, 3) != 6) ++failed; y->pVTbl->Release(y); }
        }
        root->pVTbl->Release(root);
    }
    bus->pVTbl->Release(bus); system->pVTbl->Release(system);
    printf("Official interface compatibility failures: %u\n", failed);
    return failed == 0 ? 0 : -1;
}
