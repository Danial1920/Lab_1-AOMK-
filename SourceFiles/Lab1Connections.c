/* Synchronous connection points for one polyfit instance. */
#include "Lab1Connections.h"
#include "Err.h"

struct Lab1Subscription {
    IEcoLab1Events* sink;
    uint32_t cookie;
    Lab1Subscription* next;
};

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
    return hub->source->pVTbl->QueryInterface(hub->source, iid, out);
}
static uint32_t ECOCALLMETHOD container_add(IEcoConnectionPointContainer* me) {
    Lab1Connections* hub;
    if (!me) return 0;
    hub = (Lab1Connections*)me;
    return hub->source->pVTbl->AddRef(hub->source);
}
static uint32_t ECOCALLMETHOD container_release(IEcoConnectionPointContainer* me) {
    Lab1Connections* hub;
    if (!me) return 0;
    hub = (Lab1Connections*)me;
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
    if (!same_id(iid, &IID_IEcoLab1Events)) return ERR_ECO_NOINTERFACE;
    *out = &((Lab1Connections*)me)->point.iface;
    point_add(*out);
    return 0;
}
static int16_t ECOCALLMETHOD point_iid(IEcoConnectionPoint* me, UGUID* out) {
    if (!out || !me) return ERR_ECO_POINTER;
    *out = IID_IEcoLab1Events;
    return 0;
}
static int16_t ECOCALLMETHOD point_parent(IEcoConnectionPoint* me, IEcoConnectionPointContainer** out) {
    if (!out) return ERR_ECO_POINTER;
    *out = 0;
    if (!me) return ERR_ECO_POINTER;
    *out = &((Lab1Point*)me)->owner->container;
    container_add(*out);
    return 0;
}
static int16_t ECOCALLMETHOD advise(IEcoConnectionPoint* me, IEcoUnknown* unknown, uint32_t* cookie) {
    Lab1Connections* hub;
    Lab1Subscription* node;
    IEcoLab1Events* sink = 0;
    int16_t status;
    if (!cookie) return ERR_ECO_POINTER;
    *cookie = 0;
    if (!me || !unknown) return ERR_ECO_POINTER;
    hub = ((Lab1Point*)me)->owner;
    status = unknown->pVTbl->QueryInterface(unknown, &IID_IEcoLab1Events, (voidptr_t*)&sink);
    if (status || !sink) return ERR_ECO_NOINTERFACE;
    if (hub->last_cookie == 0xffffffffu) {
        sink->pVTbl->Release(sink);
        return ERR_ECO_OUTOFMEMORY;
    }
    node = (Lab1Subscription*)hub->memory->pVTbl->Alloc(hub->memory, sizeof(*node));
    if (!node) {
        sink->pVTbl->Release(sink);
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
    IEcoLab1Events* sink;
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
        sink->pVTbl->Release(sink);
        return 0;
    }
    return ERR_ECO_VALUE_NOT_FOUND;
}

/* Enumerators have an immutable snapshot and an independent cursor per clone. */
typedef struct SnapshotItem {
    IEcoLab1Events* sink;
    uint32_t cookie;
} SnapshotItem;
typedef struct Snapshot {
    IEcoMemoryAllocator1* memory;
    IEcoConnectionPoint* point;
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
    if (snap->point) point_release(snap->point);
    for (i = 0; snap->items && i < snap->count; ++i)
        snap->items[i].sink->pVTbl->Release(snap->items[i].sink);
    if (snap->items) memory->pVTbl->Free(memory, snap->items);
    memory->pVTbl->Free(memory, snap);
    memory->pVTbl->Release(memory);
}
static IEcoEnumConnectionPointsVTbl points_vtbl;
static IEcoEnumConnectionsVTbl connections_vtbl;
static Cursor* new_cursor(Snapshot* snap, uint32_t pos) {
    Cursor* cur = (Cursor*)snap->memory->pVTbl->Alloc(snap->memory, sizeof(*cur));
    if (!cur) return 0;
    if (snap->point) cur->iface.points.pVTbl = &points_vtbl;
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
    snap->point = 0;
    snap->items = 0;
    if (points) {
        snap->point = &hub->point.iface;
        point_add(snap->point);
        snap->count = 1;
    } else {
        for (node = hub->head; node; node = node->next) if (node->sink) ++count;
        if (count) {
            if (count <= 0xffffffffu / sizeof(SnapshotItem))
                snap->items = (SnapshotItem*)mem->pVTbl->Alloc(mem, (uint32_t)(count * sizeof(SnapshotItem)));
            if (!snap->items) { drop_snapshot(snap); return ERR_ECO_OUTOFMEMORY; }
            for (node = hub->head; node; node = node->next) {
                if (!node->sink) continue;
                node->sink->pVTbl->AddRef(node->sink);
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
    if (fetched) *fetched = 0;
    if (!cur || !out || (!fetched && n != 1)) return ERR_ECO_POINTER;
    if (!n) return 0;
    *out = 0;
    if (cur->pos) return 1;
    *out = cur->snapshot->point;
    point_add(*out);
    ++cur->pos;
    if (fetched) *fetched = 1;
    return n == 1 ? 0 : 1;
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
        rc = item->sink->pVTbl->QueryInterface(item->sink, &IID_IEcoUnknown, (voidptr_t*)&out[i].pUnk);
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
    hub->memory = memory;
    hub->head = 0;
    hub->dispatch_depth = 0;
    hub->last_cookie = 0;
}
void lab1_connections_clear(Lab1Connections* hub) {
    Lab1Subscription* node;
    while (hub->head) {
        node = hub->head;
        hub->head = node->next;
        if (node->sink) node->sink->pVTbl->Release(node->sink);
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
#define DELIVER(call) \
    Lab1Subscription* node; IEcoLab1Events* sink; \
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
    DELIVER(OnFitStarted(sink, hub->source, method, count, degree));
}
void lab1_pivot(Lab1Connections* hub, uint32_t column, uint32_t row, long double pivot) {
    DELIVER(OnPivotSelected(sink, hub->source, column, row, pivot));
}
void lab1_rotation(Lab1Connections* hub, uint32_t sample, uint32_t column, long double c, long double s) {
    DELIVER(OnGivensRotation(sink, hub->source, sample, column, c, s));
}
void lab1_finished(Lab1Connections* hub, int16_t status, uint32_t count, const long double* coefficients) {
    DELIVER(OnFitFinished(sink, hub->source, status, count, coefficients));
}
