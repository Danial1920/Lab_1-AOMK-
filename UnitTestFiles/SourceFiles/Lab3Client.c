/* Public Eco client. Each source is obtained through the actual Interface Bus. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "EcoConnectionInterfaces.h"
#include "IEcoLab1Events.h"
#include "IEcoLab2Events.h"
#include "IEcoCalculatorSafe.h"
#include "IdEcoPolynomialMath.h"
#include "IdEcoLab2.h"
#include "IdEcoMemoryManager1.h"
#include "RealAllocatorProbe.h"

static unsigned checks;
#define CHECK(test) do { ++checks; if (!(test)) { \
    fprintf(stderr, "LAB3 FAIL line %d: %s\n", __LINE__, #test); exit(EXIT_FAILURE); \
} } while (0)

enum { START, PIVOT, ROTATION, FINISH };
typedef struct Event {
    IEcoPolynomialMath* source;
    int kind;
    uint32_t a, b, method;
    int16_t status;
    long double values[3];
} Event;
typedef struct Sink {
    IEcoLab1Events iface;
    IEcoLab2Events* horner;
    uint32_t refs, used;
    int verbose;
    IEcoConnectionPoint* disconnect;
    uint32_t cookie;
    Event log[64];
} Sink;

static int near_value(long double actual, long double expected) {
    long double diff = actual - expected;
    return diff < 1e-10L && diff > -1e-10L;
}
static uint32_t ECOCALLMETHOD sink_add(IEcoLab1Events* me) {
    return me ? ++((Sink*)me)->refs : 0;
}
static uint32_t ECOCALLMETHOD sink_release(IEcoLab1Events* me) {
    if (!me) return 0;
    CHECK(((Sink*)me)->refs > 0);
    return --((Sink*)me)->refs;
}
static int16_t ECOCALLMETHOD sink_qi(IEcoLab1Events* me, const UGUID* iid, voidptr_t* out) {
    if (!out) return ERR_ECO_POINTER;
    *out = 0;
    if (!me || !iid) return ERR_ECO_POINTER;
    if (((Sink*)me)->horner && !memcmp(iid,&IID_IEcoLab2Events,sizeof(*iid))) {
        sink_add(me); *out = ((Sink*)me)->horner; return 0;
    }
    if (memcmp(iid, &IID_IEcoUnknown, sizeof(*iid)) && memcmp(iid, &IID_IEcoLab1Events, sizeof(*iid)))
        return ERR_ECO_NOINTERFACE;
    sink_add(me); *out = me; return 0;
}
static Event* record(Sink* sink, IEcoPolynomialMath* source, int kind) {
    Event* event;
    CHECK(sink->used < sizeof(sink->log)/sizeof(sink->log[0]));
    event = &sink->log[sink->used++];
    event->kind = kind;
    event->source = source;
    return event;
}
static int16_t ECOCALLMETHOD started(IEcoLab1Events* me, IEcoPolynomialMath* src,
    uint32_t method, uint32_t count, uint32_t degree) {
    Sink* sink = (Sink*)me;
    Event* e = record(sink, src, START);
    e->method = method; e->a = count; e->b = degree;
    if (sink->verbose) printf("  %s start n=%u degree=%u\n", method ? "QR" : "Gauss", count, degree);
    if (sink->disconnect) {
        CHECK(sink->disconnect->pVTbl->Unadvise(sink->disconnect, sink->cookie) == 0);
        sink->disconnect = 0;
    }
    return 0;
}
static int16_t ECOCALLMETHOD pivot(IEcoLab1Events* me, IEcoPolynomialMath* src,
    uint32_t column, uint32_t row, long double value) {
    Event* e = record((Sink*)me, src, PIVOT);
    e->a = column; e->b = row; e->values[0] = value;
    if (((Sink*)me)->verbose) printf("    pivot col=%u row=%u value=%.6Lg\n", column, row, value);
    return 0;
}
static int16_t ECOCALLMETHOD rotation(IEcoLab1Events* me, IEcoPolynomialMath* src,
    uint32_t sample, uint32_t column, long double c, long double s) {
    Event* e = record((Sink*)me, src, ROTATION);
    e->a = sample; e->b = column; e->values[0] = c; e->values[1] = s;
    if (((Sink*)me)->verbose) printf("    rotate sample=%u col=%u c=%.6Lg s=%.6Lg\n", sample, column, c, s);
    return 0;
}
static int16_t ECOCALLMETHOD finished(IEcoLab1Events* me, IEcoPolynomialMath* src,
    int16_t status, uint32_t length, const long double* coeffs) {
    uint32_t i;
    Event* e = record((Sink*)me, src, FINISH);
    e->status = status; e->a = length;
    if (status) CHECK(length == 0 && coeffs == 0);
    else {
        CHECK(coeffs && length <= 3);
        for (i = 0; i < length; ++i) e->values[i] = coeffs[i];
    }
    if (((Sink*)me)->verbose && !status && length == 3)
        printf("  finish status=0 coefficients=[%.8Lg, %.8Lg, %.8Lg]\n", coeffs[0], coeffs[1], coeffs[2]);
    return 0;
}
static IEcoLab1EventsVTbl sink_vtbl = {sink_qi, sink_add, sink_release, started, pivot, rotation, finished};
static void init_sink(Sink* sink, int verbose) {
    memset(sink, 0, sizeof(*sink));
    sink->iface.pVTbl = &sink_vtbl; sink->refs = 1; sink->verbose = verbose;
}

typedef struct Rotation {
    uint32_t sample, column;
    double c, s;
} Rotation;
/* Reference rotations of [z*z,z,1] for z=-1,-.5,0,.5,1. */
static const Rotation expected_rotations[] = {
    {0,0,0,1}, {1,0,.9701425001453319,.242535625036333}, {1,1,0,-1},
    {2,0,1,0}, {2,1,1,0}, {2,2,0,1}, {3,0,.9718253158075502,.2357022603955159},
    {3,1,.3102526139970115,.9506541513652699}, {3,2,.7416198487095663,.6708203932499369},
    {4,0,.727606875108999,.6859943405700353}, {4,1,.4944132324730443,.8692269873603531},
    {4,2,.9397429877987296,-.3418817293789139}
};
static const double x[5] = {-2,-1,0,1,2}, y[5] = {11,5,3,5,11};
static void run_fit(IEcoPolynomialMath* src) {
    double out[3];
    CHECK(src->pVTbl->polyfit(src,x,y,5,2,out) == 0);
    CHECK(near_value(out[0],2) && near_value(out[1],0) && near_value(out[2],3));
}
static void verify_trace(Sink* sink, IEcoPolynomialMath* src, unsigned method) {
    unsigned i;
    Event* e;
    const unsigned rows[3] = {2,1,2};
    const double pivots[3] = {2.5,2.5,-1.75};
    CHECK(sink->used == (method ? 14u : 5u));
    for (i = 0; i < sink->used; ++i) CHECK(sink->log[i].source == src);
    e = sink->log;
    CHECK(e->kind == START && e->method == method && e->a == 5 && e->b == 2);
    for (i = 0; i < sink->used - 2; ++i) {
        e = &sink->log[i+1];
        if (method) {
            const Rotation* r = &expected_rotations[i];
            CHECK(e->kind == ROTATION && e->a == r->sample && e->b == r->column);
            CHECK(near_value(e->values[0],r->c) && near_value(e->values[1],r->s));
        } else {
            CHECK(e->kind == PIVOT && e->a == i && e->b == rows[i]);
            CHECK(near_value(e->values[0],pivots[i]));
        }
    }
    e = &sink->log[sink->used-1];
    CHECK(e->kind == FINISH && e->status == 0 && e->a == 3);
    CHECK(near_value(e->values[0],2) && near_value(e->values[1],0) && near_value(e->values[2],3));
}

/* A second polynomial checks typed entry points and event payloads
 * independently of the quadratic example. */
static void typed_paths(IEcoPolynomialMath* src, Sink* sink, unsigned method) {
    const float xf[3] = {-1,0,1}, yf[3] = {9,6,3};
    const int xi[3] = {-1,0,1}, yi[3] = {9,6,3};
    const long xl[3] = {-1,0,1}, yl[3] = {9,6,3};
    const long double xld[3] = {-1,0,1}, yld[3] = {9,6,3};
    float f[2]; double d[2]; long double ld[2];
    unsigned i;
    Event* last;
    int16_t rc;
    for (i = 0; i < 4; ++i) {
        sink->used = 0;
        if (i == 0) {
            rc = src->pVTbl->polyfitf(src,xf,yf,3,1,f);
            CHECK(!rc && near_value(f[0],-3) && near_value(f[1],6));
        } else if (i == 1) {
            rc = src->pVTbl->polyfiti(src,xi,yi,3,1,d);
            CHECK(!rc && near_value(d[0],-3) && near_value(d[1],6));
        } else {
            if (i == 2) rc = src->pVTbl->polyfit_long(src,xl,yl,3,1,ld);
            else rc = src->pVTbl->polyfitl(src,xld,yld,3,1,ld);
            CHECK(!rc && near_value(ld[0],-3) && near_value(ld[1],6));
        }
        CHECK(sink->used > 2 && sink->log[0].kind == START && sink->log[0].source == src);
        CHECK(sink->log[0].method == method && sink->log[0].a == 3 && sink->log[0].b == 1);
        last = &sink->log[sink->used-1];
        CHECK(last->kind == FINISH && last->status == 0 && last->a == 2);
        CHECK(near_value(last->values[0],-3) && near_value(last->values[1],6));
    }
    sink->used = 0;
    CHECK(src->pVTbl->polyfit(src,0,y,5,1,d) == ERR_ECO_POINTER);
    CHECK(sink->used == 2 && sink->log[0].kind == START && sink->log[1].kind == FINISH);
    CHECK(sink->log[1].status == ERR_ECO_POINTER);
}

static IEcoConnectionPoint* get_point(IEcoPolynomialMath* src) {
    IEcoConnectionPointContainer *container, *parent;
    IEcoConnectionPoint *point, *item;
    IEcoEnumConnectionPoints *en, *clone;
    uint32_t fetched;
    UGUID iid;
    CHECK(src->pVTbl->QueryInterface(src,&IID_IEcoConnectionPointContainer,(voidptr_t*)&container) == 0 && container);
    CHECK(container->pVTbl->FindConnectionPoint(container,&IID_IEcoPolynomial,&point) == ERR_ECO_NOINTERFACE && !point);
    CHECK(container->pVTbl->FindConnectionPoint(container,&IID_IEcoLab1Events,&point) == 0 && point);
    CHECK(point->pVTbl->GetConnectionInterface(point,&iid) == 0 && !memcmp(&iid,&IID_IEcoLab1Events,sizeof(iid)));
    CHECK(point->pVTbl->GetConnectionPointContainer(point,&parent) == 0 && parent == container);
    parent->pVTbl->Release(parent);
    CHECK(container->pVTbl->EnumConnectionPoints(container,&en) == 0 && en);
    CHECK(en->pVTbl->Next(en,1,&item,&fetched) == 0 && fetched == 1 && item == point);
    item->pVTbl->Release(item);
    CHECK(en->pVTbl->Clone(en,&clone) == 0 && clone);
    CHECK(clone->pVTbl->Next(clone,1,&item,&fetched) == 1 && fetched == 0);
    CHECK(en->pVTbl->Reset(en) == 0 && en->pVTbl->Skip(en,2) == 1);
    clone->pVTbl->Release(clone); en->pVTbl->Release(en);
    container->pVTbl->Release(container);
    return point;
}
static void check_snapshot(IEcoEnumConnections* snapshot, Sink sinks[2], const uint32_t cookies[2]) {
    IEcoEnumConnections* clone;
    EcoConnectionData data[2];
    uint32_t n, seen = 0, i;
    CHECK(snapshot->pVTbl->Clone(snapshot,&clone) == 0 && clone);
    CHECK(clone->pVTbl->Skip(clone,1) == 0);
    CHECK(clone->pVTbl->Next(clone,2,data,&n) == 1 && n == 1);
    data[0].pUnk->pVTbl->Release(data[0].pUnk);
    CHECK(snapshot->pVTbl->Next(snapshot,2,data,&n) == 0 && n == 2);
    for (i = 0; i < n; ++i) {
        if (data[i].pUnk == (IEcoUnknown*)&sinks[0].iface && data[i].cCookie == cookies[0]) seen |= 1;
        if (data[i].pUnk == (IEcoUnknown*)&sinks[1].iface && data[i].cCookie == cookies[1]) seen |= 2;
        data[i].pUnk->pVTbl->Release(data[i].pUnk);
    }
    CHECK(seen == 3);
    CHECK(snapshot->pVTbl->Reset(snapshot) == 0);
    CHECK(snapshot->pVTbl->Skip(snapshot,3) == 1);
    clone->pVTbl->Release(clone);
}
static void run_round(IEcoPolynomialMath* src[2], Sink sinks[2], int round) {
    unsigned i, j;
    for (i = 0; i < 2; ++i) {
        sinks[0].used = sinks[1].used = 0;
        run_fit(src[i]);
        for (j = 0; j < 2; ++j) {
            if (round == 2 && i == 1 && j == 0) CHECK(sinks[j].used == 0);
            else verify_trace(&sinks[j],src[i],i);
        }
        printf("round=%d source=%s console=%u journal=%u\n",round,i ? "QR" : "Gauss",sinks[0].used,sinks[1].used);
    }
}
static void many_to_many(IEcoInterfaceBus1* bus) {
    IEcoPolynomialMath* src[2];
    IEcoConnectionPoint* points[2];
    IEcoEnumConnections* snapshot;
    Sink sinks[2];
    uint32_t cookies[2][2], invalid = 99;
    unsigned i, j;
    const double repeated[5] = {1,1,1,1,1};
    double out[3];
    Event* last;
    init_sink(&sinks[0],1); init_sink(&sinks[1],0);
    for (i = 0; i < 2; ++i) {
        CHECK(bus->pVTbl->QueryComponent(bus,i ? &CID_EcoPolynomialMathQR : &CID_EcoPolynomialMath,
            0,&IID_IEcoPolynomialMath,(voidptr_t*)&src[i]) == 0 && src[i]);
        run_fit(src[i]);
        points[i] = get_point(src[i]);
        CHECK(points[i]->pVTbl->Advise(points[i],(IEcoUnknown*)src[i],&invalid) == ERR_ECO_NOINTERFACE && invalid == 0);
        for (j = 0; j < 2; ++j) {
            CHECK(points[i]->pVTbl->Advise(points[i],(IEcoUnknown*)&sinks[j].iface,&cookies[i][j]) == 0 && cookies[i][j]);
        }
        CHECK(cookies[i][0] != cookies[i][1]);
    }
    CHECK(points[0] != points[1] && src[0] != src[1]);
    CHECK(sinks[0].refs == 3 && sinks[1].refs == 3);
    run_round(src,sinks,1);
    CHECK(points[1]->pVTbl->EnumConnections(points[1],&snapshot) == 0 && snapshot);
    CHECK(points[1]->pVTbl->Unadvise(points[1],cookies[1][0]) == 0);
    CHECK(points[1]->pVTbl->Unadvise(points[1],cookies[1][0]) == ERR_ECO_VALUE_NOT_FOUND);
    check_snapshot(snapshot,sinks,cookies[1]);
    snapshot->pVTbl->Release(snapshot);
    CHECK(sinks[0].refs == 2 && sinks[1].refs == 3);
    run_round(src,sinks,2);
    /* Self-disconnect during a notification; subsequent steps must skip this sink. */
    sinks[0].disconnect = points[0]; sinks[0].cookie = cookies[0][0];
    sinks[0].used = sinks[1].used = 0;
    run_fit(src[0]);
    CHECK(sinks[0].used == 1 && sinks[0].log[0].kind == START && sinks[0].refs == 1);
    verify_trace(&sinks[1],src[0],0);
    for (i = 0; i < 2; ++i) {
        sinks[1].used = 0;
        CHECK(src[i]->pVTbl->polyfit(src[i],repeated,y,5,2,out) == ERR__POLYFIT_SINGULAR);
        CHECK(sinks[1].used >= 2 && sinks[1].log[0].kind == START);
        last = &sinks[1].log[sinks[1].used-1];
        CHECK(last->kind == FINISH && last->source == src[i] && last->status == ERR__POLYFIT_SINGULAR);
        typed_paths(src[i],&sinks[1],i);
        points[i]->pVTbl->Release(points[i]);
        CHECK(src[i]->pVTbl->Release(src[i]) == 0);
        CHECK(sinks[1].refs == 2u-i);
    }
    CHECK(sink_release(&sinks[0].iface) == 0 && sink_release(&sinks[1].iface) == 0);
}
typedef struct HornerSink {
    IEcoLab2Events iface;
    Sink fit;
    uint32_t steps, count;
    int phase;
    int16_t x, first, coefficient[2], status;
    int32_t product[2], partial[2], final;
} HornerSink;
static uint32_t ECOCALLMETHOD horner_add(IEcoLab2Events* me) {
    return me ? sink_add(&((HornerSink*)me)->fit.iface) : 0;
}
static uint32_t ECOCALLMETHOD horner_release(IEcoLab2Events* me) {
    return me ? sink_release(&((HornerSink*)me)->fit.iface) : 0;
}
static int16_t ECOCALLMETHOD horner_qi(IEcoLab2Events* me, const UGUID* iid, voidptr_t* out) {
    return sink_qi(me ? &((HornerSink*)me)->fit.iface : 0,iid,out);
}
static int16_t ECOCALLMETHOD horner_start(IEcoLab2Events* me, const int16_t* c, uint32_t n, int16_t x) {
    HornerSink* sink = (HornerSink*)me;
    CHECK(!sink->phase && n && c);
    sink->phase = 1; sink->first = c[0]; sink->count = n; sink->x = x; sink->steps = 0;
    return 0;
}
static int16_t ECOCALLMETHOD horner_step(IEcoLab2Events* me, uint32_t i, int32_t p, int16_t c, int32_t acc) {
    HornerSink* sink = (HornerSink*)me;
    CHECK(sink->phase == 1 && i == sink->steps + 1 && sink->steps < 2);
    sink->product[sink->steps] = p; sink->coefficient[sink->steps] = c; sink->partial[sink->steps++] = acc;
    return 0;
}
static int16_t ECOCALLMETHOD horner_finish(IEcoLab2Events* me, int32_t acc, int16_t status) {
    HornerSink* sink = (HornerSink*)me;
    CHECK(sink->phase == 1);
    sink->phase = 2; sink->status = status; sink->final = acc;
    return 0;
}
static IEcoLab2EventsVTbl horner_vtbl = {horner_qi,horner_add,horner_release,horner_start,horner_step,horner_finish};

static void aggregate_identity(IEcoInterfaceBus1* bus) {
    IEcoUnknown *root, *identity;
    IEcoLab2* lab2;
    IEcoPolynomialMath* math;
    IEcoConnectionPoint *points[2], *items[2];
    IEcoConnectionPointContainer *container, *parent;
    IEcoEnumConnectionPoints *en, *clone;
    const UGUID* expected[2];
    UGUID iid;
    HornerSink horner;
    IEcoEnumConnections* subscribers;
    EcoConnectionData subscriber;
    int16_t coefficients[] = {2,-3,4}, overflow[] = {200,1,1};
    uint32_t cookies[2], fetched, i;
    int32_t result;
    expected[0] = &IID_IEcoLab2Events; expected[1] = &IID_IEcoLab1Events;
    memset(&horner,0,sizeof(horner)); horner.iface.pVTbl = &horner_vtbl; init_sink(&horner.fit,0);
    horner.fit.horner = &horner.iface;
    CHECK(bus->pVTbl->QueryComponent(bus,&CID_EcoLab2,0,&IID_IEcoUnknown,(voidptr_t*)&root) == 0 && root);
    CHECK(root->pVTbl->QueryInterface(root,&IID_IEcoLab2,(voidptr_t*)&lab2) == 0 && lab2);
    CHECK(root->pVTbl->QueryInterface(root,&IID_IEcoPolynomialMath,(voidptr_t*)&math) == 0 && math);
    CHECK(math->pVTbl->QueryInterface(math,&IID_IEcoConnectionPointContainer,(voidptr_t*)&container) == 0 && container);
    for (i = 0; i < 2; ++i) {
        CHECK(container->pVTbl->FindConnectionPoint(container,expected[i],&points[i]) == 0 && points[i]);
        CHECK(points[i]->pVTbl->GetConnectionInterface(points[i],&iid) == 0 && !memcmp(&iid,expected[i],sizeof(iid)));
        CHECK(points[i]->pVTbl->GetConnectionPointContainer(points[i],&parent) == 0 && parent == container);
        CHECK(parent->pVTbl->QueryInterface(parent,&IID_IEcoUnknown,(voidptr_t*)&identity) == 0 && identity == root);
        identity->pVTbl->Release(identity); parent->pVTbl->Release(parent);
    }
    CHECK(container->pVTbl->EnumConnectionPoints(container,&en) == 0 && en);
    CHECK(en->pVTbl->Next(en,2,items,&fetched) == 0 && fetched == 2 && items[0] == points[0] && items[1] == points[1]);
    for (i = 0; i < 2; ++i) items[i]->pVTbl->Release(items[i]);
    CHECK(en->pVTbl->Next(en,1,items,&fetched) == 1 && !fetched);
    CHECK(en->pVTbl->Reset(en) == 0 && en->pVTbl->Skip(en,1) == 0 && en->pVTbl->Clone(en,&clone) == 0);
    CHECK(clone->pVTbl->Next(clone,1,items,&fetched) == 0 && fetched == 1 && items[0] == points[1]);
    items[0]->pVTbl->Release(items[0]); clone->pVTbl->Release(clone); en->pVTbl->Release(en);
    CHECK(points[0]->pVTbl->Advise(points[0],(IEcoUnknown*)math,&cookies[0]) != 0 && !cookies[0]);
    CHECK(points[0]->pVTbl->Advise(points[0],(IEcoUnknown*)&horner.fit.iface,&cookies[0]) == 0 && cookies[0]);
    CHECK(points[1]->pVTbl->Advise(points[1],(IEcoUnknown*)&horner.fit.iface,&cookies[1]) == 0 && cookies[1]);
    CHECK(points[0]->pVTbl->EnumConnections(points[0],&subscribers) == 0 && subscribers);
    CHECK(subscribers->pVTbl->Next(subscribers,1,&subscriber,&fetched) == 0 && fetched == 1);
    CHECK(subscriber.pUnk == (IEcoUnknown*)&horner.fit.iface && subscriber.cCookie == cookies[0]);
    subscriber.pUnk->pVTbl->Release(subscriber.pUnk);
    subscribers->pVTbl->Release(subscribers);
    CHECK(lab2->pVTbl->Horner(lab2,coefficients,0,5,&result) != 0 && !horner.phase);
    CHECK(lab2->pVTbl->Horner(lab2,coefficients,3,5,&result) == 0 && result == 39 && !horner.fit.used);
    CHECK(horner.phase == 2 && horner.count == 3 && horner.x == 5 && horner.first == 2 && horner.steps == 2);
    CHECK(horner.product[0] == 10 && horner.coefficient[0] == -3 && horner.partial[0] == 7);
    CHECK(horner.product[1] == 35 && horner.coefficient[1] == 4 && horner.partial[1] == 39);
    CHECK(horner.final == result && !horner.status);
    printf("Horner x=%d: partials=[%d, %d], result=%d\n",horner.x,(int)horner.partial[0],(int)horner.partial[1],(int)result);
    horner.phase = 0;
    CHECK(lab2->pVTbl->Horner(lab2,overflow,3,100,&result) == ERR_ECO_CALCULATOR_OVERFLOW && !result);
    CHECK(horner.phase == 2 && horner.steps == 1 && horner.final == 20001 && horner.status == ERR_ECO_CALCULATOR_OVERFLOW);
    CHECK(points[0]->pVTbl->Unadvise(points[0],cookies[0]) == 0 && horner.fit.refs == 2);
    horner.phase = 0;
    CHECK(lab2->pVTbl->Horner(lab2,coefficients,3,5,&result) == 0 && result == 39 && !horner.phase);
    run_fit(math); verify_trace(&horner.fit,math,0);
    container->pVTbl->Release(container); lab2->pVTbl->Release(lab2);
    math->pVTbl->Release(math); root->pVTbl->Release(root);
    points[0]->pVTbl->Release(points[0]);
    CHECK(points[1]->pVTbl->Release(points[1]) == 0 && horner.fit.refs == 1);
    CHECK(horner_release(&horner.iface) == 0);
}
/* All seven failures below are injected into the real Eco allocator after
 * creating the source. Each return is checked while the object stays usable. */
static void allocation_failures(IEcoInterfaceBus1* bus) {
    IEcoMemoryAllocator1* memory;
    IEcoPolynomialMath* src;
    IEcoConnectionPointContainer* container;
    IEcoConnectionPoint* point;
    IEcoEnumConnectionPoints* points;
    IEcoEnumConnections *en, *clone;
    Sink sink;
    uint32_t cookie, fail, baseline;
    init_sink(&sink,0);
    CHECK(bus->pVTbl->QueryComponent(bus,&CID_EcoMemoryManager1,0,&IID_IEcoMemoryAllocator1,
        (voidptr_t*)&memory) == 0 && memory);
    CHECK(bus->pVTbl->QueryComponent(bus,&CID_EcoPolynomialMath,0,&IID_IEcoPolynomialMath,
        (voidptr_t*)&src) == 0 && src);
    point = get_point(src);
    CHECK(point->pVTbl->GetConnectionPointContainer(point,&container) == 0);
    probe_start(memory);
    baseline = probe_reference_count();
    real_memory.fail_at = 1;
    CHECK(point->pVTbl->Advise(point,(IEcoUnknown*)&sink.iface,&cookie) == ERR_ECO_OUTOFMEMORY);
    CHECK(!cookie && sink.refs == 1 && real_memory.live == 0);
    real_memory.fail_at = 0;
    CHECK(point->pVTbl->Advise(point,(IEcoUnknown*)&sink.iface,&cookie) == 0);
    for (fail = 1; fail <= 2; ++fail) {
        real_memory.calls = 0; real_memory.fail_at = fail;
        CHECK(container->pVTbl->EnumConnectionPoints(container,&points) == ERR_ECO_OUTOFMEMORY && !points);
        CHECK(real_memory.live == 1 && probe_reference_count() == baseline);
    }
    for (fail = 1; fail <= 3; ++fail) {
        real_memory.calls = 0; real_memory.fail_at = fail;
        CHECK(point->pVTbl->EnumConnections(point,&en) == ERR_ECO_OUTOFMEMORY && !en);
        CHECK(sink.refs == 2 && real_memory.live == 1 && probe_reference_count() == baseline);
    }
    real_memory.fail_at = 0;
    CHECK(point->pVTbl->EnumConnections(point,&en) == 0 && en);
    real_memory.calls = 0; real_memory.fail_at = 1;
    CHECK(en->pVTbl->Clone(en,&clone) == ERR_ECO_OUTOFMEMORY && !clone);
    real_memory.fail_at = 0; en->pVTbl->Release(en);
    run_fit(src);
    verify_trace(&sink,src,0);
    CHECK(point->pVTbl->Unadvise(point,cookie) == 0 && sink.refs == 1);
    CHECK(real_memory.invalid == 0 && real_memory.live == 0 && real_memory.allocations == real_memory.frees);
    CHECK(probe_reference_count() == baseline);
    printf("Allocation audit: 7 injected failures, allocations=%u frees=%u live=%u\n",
        real_memory.allocations,real_memory.frees,real_memory.live);
    probe_stop(); memory->pVTbl->Release(memory);
    container->pVTbl->Release(container); point->pVTbl->Release(point);
    CHECK(src->pVTbl->Release(src) == 0);
    CHECK(sink_release(&sink.iface) == 0);
}
#ifndef ECO_DYNAMIC_CLIENT
static void register_components(IEcoInterfaceBus1* bus) {
    const UGUID* ids[7];
    IEcoComponentFactory* factories[7];
    unsigned i;
    ids[0] = &CID_EcoPolynomialMath; factories[0] = GetPolynomialFactory_00000045();
    ids[1] = &CID_EcoPolynomialMathQR; factories[1] = GetPolynomialQRFactory_00000046();
    ids[2] = &CID_EcoLab2; factories[2] = GetEcoLab2Factory();
    ids[3] = &CID_EcoCalculatorA; factories[3] = GetCalculatorAFactory();
    ids[4] = &CID_EcoCalculatorB; factories[4] = GetCalculatorBFactory();
    ids[5] = &CID_EcoCalculatorD; factories[5] = GetCalculatorDFactory();
    ids[6] = &CID_EcoCalculatorE; factories[6] = GetCalculatorEFactory();
    for (i = 0; i < 7; ++i)
        CHECK(bus->pVTbl->RegisterComponent(bus,ids[i],(IEcoUnknown*)factories[i]) == 0);
}
#endif
int16_t EcoMain(IEcoUnknown* entry) {
    IEcoSystem1* system;
    IEcoInterfaceBus1* bus;
    CHECK(entry && entry->pVTbl->QueryInterface(entry,&GID_IEcoSystem,(voidptr_t*)&system) == 0 && system);
    CHECK(system->pVTbl->QueryInterface(system,&IID_IEcoInterfaceBus1,(voidptr_t*)&bus) == 0 && bus);
#ifndef ECO_DYNAMIC_CLIENT
    register_components(bus);
#endif
    many_to_many(bus);
    aggregate_identity(bus);
    allocation_failures(bus);
    bus->pVTbl->Release(bus); system->pVTbl->Release(system);
    printf("LAB3 PASS: %u checks\n",checks);
    return 0;
}
