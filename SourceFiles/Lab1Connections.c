/* Synchronous connection points shared by the numerical and Horner components. */
#include "Lab1Connections.h"
#include "Err.h"

struct Lab1Subscription {
    void* sink;
    uint32_t cookie;
    Lab1Subscription* next;
};

static void sink_add(void* sink, int horner) {
    if (horner) ((IEcoLab2Events*)sink)->pVTbl->AddRef((IEcoLab2Events*)sink);
    else ((IEcoLab1Events*)sink)->pVTbl->AddRef((IEcoLab1Events*)sink);
}
static void sink_release(void* sink, int horner) {
    if (horner) ((IEcoLab2Events*)sink)->pVTbl->Release((IEcoLab2Events*)sink);
    else ((IEcoLab1Events*)sink)->pVTbl->Release((IEcoLab1Events*)sink);
}
static int16_t sink_identity(void* sink, int horner, IEcoUnknown** out) {
    if (horner) return ((IEcoLab2Events*)sink)->pVTbl->QueryInterface(
        (IEcoLab2Events*)sink, &IID_IEcoUnknown, (voidptr_t*)out);
    return ((IEcoLab1Events*)sink)->pVTbl->QueryInterface(
        (IEcoLab1Events*)sink, &IID_IEcoUnknown, (voidptr_t*)out);
}

static int same_id(const UGUID* a, const UGUID* b) {
    uint32_t i;
    for (i = 0; i < sizeof(*a); ++i)
        if (((const byte_t*)a)[i] != ((const byte_t*)b)[i]) return 0;
    return 1;
}

static int16_t ECOCALLMETHOD container_qi(IEcoConnectionPointContainer* me,
    const UGUID* iid, voidptr_t* out) {
    Lab1Connections* hub;
    if (!out) return ERR_ECO_POINTER;
    *out = 0;
    if (!me || !iid) return ERR_ECO_POINTER;
    hub = (Lab1Connections*)me;
    if (hub->lab2) return hub->lab2->pVTbl->QueryInterface(hub->lab2, iid, out);
    return hub->source->pVTbl->QueryInterface(hub->source, iid, out);
}
static uint32_t ECOCALLMETHOD container_add(IEcoConnectionPointContainer* me) {
    Lab1Connections* hub;
    if (!me) return 0;
    hub = (Lab1Connections*)me;
    if (hub->lab2) return hub->lab2->pVTbl->AddRef(hub->lab2);
    return hub->source->pVTbl->AddRef(hub->source);
}
static uint32_t ECOCALLMETHOD container_release(IEcoConnectionPointContainer* me) {
    Lab1Connections* hub;
    if (!me) return 0;
    hub = (Lab1Connections*)me;
    if (hub->lab2) return hub->lab2->pVTbl->Release(hub->lab2);
    return hub->source->pVTbl->Release(hub->source);
}
static uint32_t ECOCALLMETHOD point_add(IEcoConnectionPoint* me) {
    if (!me) return 0;
    return container_add(&((Lab1Point*)me)->owner->container);
}
static uint32_t ECOCALLMETHOD point_release(IEcoConnectionPoint* me) {
    if (!me) return 0;
    return container_release(&((Lab1Point*)me)->owner->container);
}
static int16_t ECOCALLMETHOD point_qi(IEcoConnectionPoint* me, const UGUID* iid, voidptr_t* out) {
    if (!out) return ERR_ECO_POINTER;
    *out = 0;
    if (!me || !iid) return ERR_ECO_POINTER;
    if (!same_id(iid, &IID_IEcoUnknown) && !same_id(iid, &IID_IEcoConnectionPoint))
        return ERR_ECO_NOINTERFACE;
    point_add(me);
    *out = me;
    return 0;
}
static int16_t ECOCALLMETHOD find_point(IEcoConnectionPointContainer* me,
    const UGUID* iid, IEcoConnectionPoint** out) {
    if (!out) return ERR_ECO_POINTER;
    *out = 0;
    if (!me || !iid) return ERR_ECO_POINTER;
    if (!same_id(iid, ((Lab1Connections*)me)->event_iid)) {
        Lab1Connections* hub = (Lab1Connections*)me;
        IEcoConnectionPointContainer* child = 0;
        int16_t status;
        if (!hub->inner || !same_id(iid, &IID_IEcoLab1Events)) return ERR_ECO_NOINTERFACE;
        status = hub->inner->pVTbl->QueryInterface(hub->inner, &IID_IEcoConnectionPointContainer,
            (voidptr_t*)&child);
        if (status || !child) return status ? status : ERR_ECO_NOINTERFACE;
        status = child->pVTbl->FindConnectionPoint(child, iid, out);
        child->pVTbl->Release(child);
        return status;
    }
    *out = &((Lab1Connections*)me)->point.iface;
    point_add(*out);
    return 0;
}
static int16_t ECOCALLMETHOD point_iid(IEcoConnectionPoint* me, UGUID* out) {
    if (!out || !me) return ERR_ECO_POINTER;
    *out = *(((Lab1Point*)me)->owner->event_iid);
    return 0;
}
static int16_t ECOCALLMETHOD point_parent(IEcoConnectionPoint* me, IEcoConnectionPointContainer** out) {
    if (!out) return ERR_ECO_POINTER;
    *out = 0;
    if (!me) return ERR_ECO_POINTER;
    return container_qi(&((Lab1Point*)me)->owner->container, &IID_IEcoConnectionPointContainer, (voidptr_t*)out);
}
static int16_t ECOCALLMETHOD advise(IEcoConnectionPoint* me, IEcoUnknown* unknown, uint32_t* cookie) {
    Lab1Connections* hub;
    Lab1Subscription* node;
    void* sink = 0;
    int16_t status;
    if (!cookie) return ERR_ECO_POINTER;
    *cookie = 0;
    if (!me || !unknown) return ERR_ECO_POINTER;
    hub = ((Lab1Point*)me)->owner;
    status = unknown->pVTbl->QueryInterface(unknown, hub->event_iid, (voidptr_t*)&sink);
    if (status || !sink) return ERR_ECO_NOINTERFACE;
    if (hub->last_cookie == 0xffffffffu) {
        sink_release(sink, hub->lab2 != 0);
        return ERR_ECO_OUTOFMEMORY;
    }
    node = (Lab1Subscription*)hub->memory->pVTbl->Alloc(hub->memory, sizeof(*node));
    if (!node) {
        sink_release(sink, hub->lab2 != 0);
        return ERR_ECO_OUTOFMEMORY;
    }
    node->cookie = ++hub->last_cookie;
    node->sink = sink;
    node->next = hub->head;
    hub->head = node;
    *cookie = node->cookie;
    return 0;
}
static int16_t ECOCALLMETHOD unadvise(IEcoConnectionPoint* me, uint32_t cookie) {
    Lab1Connections* hub;
    Lab1Subscription **link, *node;
    void* sink;
    if (!me) return ERR_ECO_POINTER;
    hub = ((Lab1Point*)me)->owner;
    for (link = &hub->head; *link; link = &(*link)->next) {
        node = *link;
        if (node->cookie != cookie || !node->sink) continue;
        sink = node->sink;
        node->sink = 0;
        if (!hub->dispatch_depth) {
            *link = node->next;
            hub->memory->pVTbl->Free(hub->memory, node);
        }
        sink_release(sink, hub->lab2 != 0);
        return 0;
    }
    return ERR_ECO_VALUE_NOT_FOUND;
}

/* Enumerators have an immutable snapshot and an independent cursor per clone. */
typedef struct SnapshotItem {
    void* sink;
    uint32_t cookie;
} SnapshotItem;
typedef struct Snapshot {
    IEcoMemoryAllocator1* memory;
    IEcoConnectionPoint* points[2];
    int horner;
    SnapshotItem* items;
    uint32_t count, refs;
} Snapshot;
typedef struct Cursor {
    union {
        IEcoEnumConnections connections;
        IEcoEnumConnectionPoints points;
    } iface;
    Snapshot* snapshot;
    uint32_t pos, refs;
} Cursor;

static void drop_snapshot(Snapshot* snap) {
    uint32_t i;
    IEcoMemoryAllocator1* memory = snap->memory;
    if (--snap->refs) return;
    for (i = 0; snap->points[0] && i < snap->count; ++i)
        snap->points[i]->pVTbl->Release(snap->points[i]);
    for (i = 0; snap->items && i < snap->count; ++i)
        sink_release(snap->items[i].sink, snap->horner);
    if (snap->items) memory->pVTbl->Free(memory, snap->items);
    memory->pVTbl->Free(memory, snap);
    memory->pVTbl->Release(memory);
}
static IEcoEnumConnectionPointsVTbl points_vtbl;
static IEcoEnumConnectionsVTbl connections_vtbl;
static Cursor* new_cursor(Snapshot* snap, uint32_t pos) {
    Cursor* cur = (Cursor*)snap->memory->pVTbl->Alloc(snap->memory, sizeof(*cur));
    if (!cur) return 0;
    if (snap->points[0]) cur->iface.points.pVTbl = &points_vtbl;
    else cur->iface.connections.pVTbl = &connections_vtbl;
    cur->snapshot = snap;
    ++snap->refs;
    cur->pos = pos;
    cur->refs = 1;
    return cur;
}
static int16_t enumerate(Lab1Connections* hub, int points, Cursor** out) {
    Snapshot* snap;
    Lab1Subscription* node;
    uint32_t count = 0;
    IEcoMemoryAllocator1* mem = hub->memory;
    snap = (Snapshot*)mem->pVTbl->Alloc(mem, sizeof(*snap));
    if (!snap) return ERR_ECO_OUTOFMEMORY;
    mem->pVTbl->AddRef(mem);
    snap->memory = mem;
    snap->refs = 1;
    snap->count = 0;
    snap->points[0] = 0;
    snap->horner = hub->lab2 != 0;
    snap->items = 0;
    if (points) {
        int16_t status;
        snap->points[0] = &hub->point.iface;
        point_add(snap->points[0]);
        snap->count = 1;
        if (hub->inner) {
            status = find_point(&hub->container, &IID_IEcoLab1Events, &snap->points[1]);
            if (status) { drop_snapshot(snap); return status; }
            ++snap->count;
        }
    } else {
        for (node = hub->head; node; node = node->next) if (node->sink) ++count;
        if (count) {
            if (count <= 0xffffffffu / sizeof(SnapshotItem))
                snap->items = (SnapshotItem*)mem->pVTbl->Alloc(mem, (uint32_t)(count * sizeof(SnapshotItem)));
            if (!snap->items) { drop_snapshot(snap); return ERR_ECO_OUTOFMEMORY; }
            for (node = hub->head; node; node = node->next) {
                if (!node->sink) continue;
                sink_add(node->sink, snap->horner);
                snap->items[snap->count].sink = node->sink;
                snap->items[snap->count++].cookie = node->cookie;
            }
        }
    }
    *out = new_cursor(snap, 0);
    drop_snapshot(snap);
    return *out ? 0 : ERR_ECO_OUTOFMEMORY;
}
static int16_t ECOCALLMETHOD enum_points(IEcoConnectionPointContainer* me, IEcoEnumConnectionPoints** out) {
    Cursor* cur = 0;
    int16_t rc;
    if (!out) return ERR_ECO_POINTER;
    *out = 0;
    if (!me) return ERR_ECO_POINTER;
    rc = enumerate((Lab1Connections*)me, 1, &cur);
    if (cur) *out = &cur->iface.points;
    return rc;
}
static int16_t ECOCALLMETHOD enum_connections(IEcoConnectionPoint* me, IEcoEnumConnections** out) {
    Cursor* cur = 0;
    int16_t rc;
    if (!out) return ERR_ECO_POINTER;
    *out = 0;
    if (!me) return ERR_ECO_POINTER;
    rc = enumerate(((Lab1Point*)me)->owner, 0, &cur);
    if (cur) *out = &cur->iface.connections;
    return rc;
}
static uint32_t cursor_release(Cursor* cur) {
    Snapshot* snap;
    if (!cur) return 0;
    if (--cur->refs) return cur->refs;
    snap = cur->snapshot;
    snap->memory->pVTbl->Free(snap->memory, cur);
    drop_snapshot(snap);
    return 0;
}
#define CURSOR_METHODS(prefix, I, field) \
static int16_t ECOCALLMETHOD prefix##_qi(I* me, const UGUID* iid, voidptr_t* out) { \
    if (!out) return ERR_ECO_POINTER; \
    *out = 0; \
    if (!me || !iid) return ERR_ECO_POINTER; \
    if (!same_id(iid, &IID_##I) && !same_id(iid, &IID_IEcoUnknown)) return ERR_ECO_NOINTERFACE; \
    ++((Cursor*)me)->refs; *out = me; return 0; \
} \
static uint32_t ECOCALLMETHOD prefix##_add(I* me) { return me ? ++((Cursor*)me)->refs : 0; } \
static uint32_t ECOCALLMETHOD prefix##_release(I* me) { return cursor_release((Cursor*)me); } \
static int16_t ECOCALLMETHOD prefix##_skip(I* me, uint32_t count) { \
    Cursor* cur = (Cursor*)me; \
    if (!cur) return ERR_ECO_POINTER; \
    if (count > cur->snapshot->count - cur->pos) { cur->pos = cur->snapshot->count; return 1; } \
    cur->pos += count; return 0; \
} \
static int16_t ECOCALLMETHOD prefix##_reset(I* me) { \
    if (!me) return ERR_ECO_POINTER; \
    ((Cursor*)me)->pos = 0; return 0; \
} \
static int16_t ECOCALLMETHOD prefix##_clone(I* me, I** out) { \
    Cursor* cur = (Cursor*)me; Cursor* copy; \
    if (!out) return ERR_ECO_POINTER; \
    *out = 0; \
    if (!cur) return ERR_ECO_POINTER; \
    copy = new_cursor(cur->snapshot, cur->pos); \
    if (!copy) return ERR_ECO_OUTOFMEMORY; \
    *out = &copy->iface.field; return 0; \
}
CURSOR_METHODS(points, IEcoEnumConnectionPoints, points)
CURSOR_METHODS(connections, IEcoEnumConnections, connections)

static int16_t ECOCALLMETHOD points_next(IEcoEnumConnectionPoints* me, uint32_t n,
    IEcoConnectionPoint** out, uint32_t* fetched) {
    Cursor* cur = (Cursor*)me;
    uint32_t i = 0;
    if (fetched) *fetched = 0;
    if (!cur || !out || (!fetched && n != 1)) return ERR_ECO_POINTER;
    if (n) *out = 0;
    while (i < n && cur->pos < cur->snapshot->count) {
        out[i] = cur->snapshot->points[cur->pos++];
        out[i]->pVTbl->AddRef(out[i]);
        ++i;
    }
    if (fetched) *fetched = i;
    return i == n ? 0 : 1;
}
static int16_t ECOCALLMETHOD connections_next(IEcoEnumConnections* me, uint32_t n,
    EcoConnectionData* out, uint32_t* fetched) {
    Cursor* cur = (Cursor*)me;
    SnapshotItem* item;
    uint32_t i = 0;
    int16_t rc;
    if (fetched) *fetched = 0;
    if (!cur || !out || (!fetched && n != 1)) return ERR_ECO_POINTER;
    while (i < n && cur->pos < cur->snapshot->count) {
        item = &cur->snapshot->items[cur->pos];
        rc = sink_identity(item->sink, cur->snapshot->horner, &out[i].pUnk);
        if (rc) return rc;
        out[i++].cCookie = item->cookie;
        ++cur->pos;
        if (fetched) *fetched = i;
    }
    return i == n ? 0 : 1;
}
static IEcoEnumConnectionPointsVTbl points_vtbl = {
    points_qi, points_add, points_release, points_next, points_skip, points_reset, points_clone
};
static IEcoEnumConnectionsVTbl connections_vtbl = {
    connections_qi, connections_add, connections_release, connections_next,
    connections_skip, connections_reset, connections_clone
};
static IEcoConnectionPointContainerVTbl container_vtbl = {
    container_qi, container_add, container_release, enum_points, find_point
};
static IEcoConnectionPointVTbl point_vtbl = {
    point_qi, point_add, point_release, point_iid, point_parent, advise, unadvise, enum_connections
};

void lab1_connections_init(Lab1Connections* hub, IEcoPolynomialMath* source, IEcoMemoryAllocator1* memory) {
    hub->container.pVTbl = &container_vtbl;
    hub->point.iface.pVTbl = &point_vtbl;
    hub->point.owner = hub;
    hub->source = source;
    hub->lab2 = 0;
    hub->inner = 0;
    hub->event_iid = &IID_IEcoLab1Events;
    hub->memory = memory;
    hub->head = 0;
    hub->dispatch_depth = 0;
    hub->last_cookie = 0;
}
void lab2_connections_init(Lab1Connections* hub, IEcoLab2* source, IEcoMemoryAllocator1* memory) {
    lab1_connections_init(hub, 0, memory);
    hub->lab2 = source;
    hub->event_iid = &IID_IEcoLab2Events;
}
void lab1_connections_clear(Lab1Connections* hub) {
    Lab1Subscription* node;
    while (hub->head) {
        node = hub->head;
        hub->head = node->next;
        if (node->sink) sink_release(node->sink, hub->lab2 != 0);
        hub->memory->pVTbl->Free(hub->memory, node);
    }
}

/* New subscriptions go at the head and start on the next event. Removed
 * nodes stay in place until the outermost callback returns. A temporary
 * reference lets a sink safely disconnect itself during notification. */
static void end_dispatch(Lab1Connections* hub) {
    Lab1Subscription **link, *node;
    if (--hub->dispatch_depth) return;
    link = &hub->head;
    while (*link) {
        node = *link;
        if (node->sink) link = &node->next;
        else {
            *link = node->next;
            hub->memory->pVTbl->Free(hub->memory, node);
        }
    }
}
#define DELIVER(type, call) \
    Lab1Subscription* node; type* sink; \
    if (!hub->head) return; \
    ++hub->dispatch_depth; \
    for (node = hub->head; node; node = node->next) { \
        sink = node->sink; \
        if (!sink) continue; \
        sink->pVTbl->AddRef(sink); \
        sink->pVTbl->call; \
        sink->pVTbl->Release(sink); \
    } \
    end_dispatch(hub)
void lab1_started(Lab1Connections* hub, uint32_t method, uint32_t count, uint32_t degree) {
    DELIVER(IEcoLab1Events, OnFitStarted(sink, hub->source, method, count, degree));
}
void lab1_pivot(Lab1Connections* hub, uint32_t column, uint32_t row, long double pivot) {
    DELIVER(IEcoLab1Events, OnPivotSelected(sink, hub->source, column, row, pivot));
}
void lab1_rotation(Lab1Connections* hub, uint32_t sample, uint32_t column, long double c, long double s) {
    DELIVER(IEcoLab1Events, OnGivensRotation(sink, hub->source, sample, column, c, s));
}
void lab1_finished(Lab1Connections* hub, int16_t status, uint32_t count, const long double* coefficients) {
    DELIVER(IEcoLab1Events, OnFitFinished(sink, hub->source, status, count, coefficients));
}

void lab2_horner_started(Lab1Connections* hub, const int16_t* coeff, uint32_t n, int16_t x) {
    DELIVER(IEcoLab2Events, OnHornerStarted(sink, coeff, n, x));
}
void lab2_horner_step(Lab1Connections* hub, uint32_t index, int32_t product, int16_t coeff, int32_t value) {
    DELIVER(IEcoLab2Events, OnHornerStep(sink, index, product, coeff, value));
}
void lab2_horner_finished(Lab1Connections* hub, int32_t value, int16_t status) {
    DELIVER(IEcoLab2Events, OnHornerFinished(sink, value, status));
}
