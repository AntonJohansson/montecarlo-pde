// A few macros follow that given the index of a node,
// returns the left or right child of that node, or
// the parent of that node. Hopefullly the following
// diagram is somewhat illustrative
//
// index | 0 |  1 |  2 |  3 |  4 |
// label | R | L0 | R0 | L1 | R1 | ...
//
//      	 R
//              / \
//             L0 R0
//            /  \
//           L1  R1
//

#define PQ_PARENT(x)    (((i32)x-1)/2)
#define PQ_LEFT(x)      (2*(i32)x+1)
#define PQ_RIGHT(x)     (2*(i32)x+2)

// Should return true if a comes before b
typedef bool pq_cmpfunc(void *a, void *b);

struct prioqueue {
    u32 items;
    u32 item_size;
    u32 max_items;

    void* memory;

    pq_cmpfunc* cmp;
};

static inline void *prioqueue_at(struct prioqueue *pq, u32 index) {
    return (u8 *) pq->memory + index * pq->item_size;
}

static void prioqueue_heapify(struct prioqueue* pq, u32 index);

static void prioqueue_heapify(struct prioqueue *pq, u32 index) {
    if (pq->items <= 1)
        return;

    u32 L = PQ_LEFT(index);
    u32 R = PQ_RIGHT(index);
    u32 tmp = index;

    void *left     = prioqueue_at(pq, L);
    void *right    = prioqueue_at(pq, R);
    void *indexptr = prioqueue_at(pq, index);

    if (L < pq->items) {
        if (!pq->cmp(indexptr, left)) {
            indexptr = left;
            index = L;
        }
    }

    if (R < pq->items) {
        if (!pq->cmp(indexptr, right)) {
            indexptr = right;
            index = R;
        }
    }

    if (index != tmp) {
        u8 tmpbuf[pq->item_size];
        memcpy(tmpbuf,                  prioqueue_at(pq, tmp),   pq->item_size);
        memcpy(prioqueue_at(pq, tmp),   prioqueue_at(pq, index), pq->item_size);
        memcpy(prioqueue_at(pq, index), tmpbuf,                  pq->item_size);

        prioqueue_heapify(pq, index);
    }
}

static void prioqueue_push(struct prioqueue *pq, const void *data) {
    assert(pq->items < pq->max_items);

    // Copy data to the last slot in the array
    u32 i = pq->items;
    memcpy(prioqueue_at(pq, i), data, pq->item_size);
    pq->items++;

    void *child  = prioqueue_at(pq, i);
    void *parent = prioqueue_at(pq, PQ_PARENT(i));
    u8 tmpbuf[pq->item_size];
    // While the child comes before the parent
    while (i != 0 && pq->cmp(child, parent)) {
        // swap child and parent
        memcpy(tmpbuf, child,  pq->item_size);
        memcpy(child,  parent, pq->item_size);
        memcpy(parent, tmpbuf, pq->item_size);

        // update indices
        i = PQ_PARENT(i);
        child  = prioqueue_at(pq, i);
        parent = prioqueue_at(pq, PQ_PARENT(i));
    }
}

static inline u8 *prioqueue_top(struct prioqueue* pq) {
    return (u8*) prioqueue_at(pq, 0);
}

static bool prioqueue_pop(struct prioqueue *pq, void *out) {
    if (pq->items == 0)
        return false;

    if (out)
        memcpy(out, prioqueue_at(pq, 0), pq->item_size);

    // swap first and last element
    u8 tmpbuf[pq->item_size];
    memcpy(tmpbuf,                        prioqueue_at(pq, 0),           pq->item_size);
    memcpy(prioqueue_at(pq, 0),           prioqueue_at(pq, pq->items-1), pq->item_size);
    memcpy(prioqueue_at(pq, pq->items-1), tmpbuf,                        pq->item_size);

    pq->items--;
    prioqueue_heapify(pq, 0);

    return true;
}
