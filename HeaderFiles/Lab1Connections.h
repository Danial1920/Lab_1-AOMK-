#ifndef LAB1_CONNECTIONS_H
#define LAB1_CONNECTIONS_H
#include "IEcoLab1Events.h"
#include "IEcoLab2Events.h"
#include "IEcoLab2.h"
#include "EcoConnectionInterfaces.h"
#include "IEcoMemoryAllocator1.h"

typedef struct Lab1Subscription Lab1Subscription;
typedef struct Lab1Point {
    IEcoConnectionPoint iface;
    struct Lab1Connections* owner;
} Lab1Point;
typedef struct Lab1Connections {
    IEcoConnectionPointContainer container;
    Lab1Point point;
    IEcoPolynomialMath* source;
    IEcoLab2* lab2;
    /* Borrowed nondelegating unknown, owned by the containing Lab2. */
    IEcoUnknown* inner;
    const UGUID* event_iid;
    IEcoMemoryAllocator1* memory;
    Lab1Subscription* head;
    uint32_t last_cookie;
    uint32_t dispatch_depth;
} Lab1Connections;
void lab1_connections_init(Lab1Connections*, IEcoPolynomialMath*, IEcoMemoryAllocator1*);
void lab2_connections_init(Lab1Connections*, IEcoLab2*, IEcoMemoryAllocator1*);
void lab1_connections_clear(Lab1Connections*);
void lab1_started(Lab1Connections*, uint32_t method, uint32_t count, uint32_t degree);
void lab1_pivot(Lab1Connections*, uint32_t column, uint32_t row, long double value);
void lab1_rotation(Lab1Connections*, uint32_t sample, uint32_t column, long double c, long double s);
void lab1_finished(Lab1Connections*, int16_t status, uint32_t count, const long double* coefficients);
void lab2_horner_started(Lab1Connections*, const int16_t*, uint32_t, int16_t);
void lab2_horner_step(Lab1Connections*, uint32_t, int32_t, int16_t, int32_t);
void lab2_horner_finished(Lab1Connections*, int32_t, int16_t);
#endif
