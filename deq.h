#ifndef DEQ_H
#define DEQ_H

// put: append onto an end, len++
// get: return from an end, len--
// ith: return by 0-base index, len unchanged
// rem: return by == comparing, len-- (iff found)

typedef void *Deq; // opaque double-ended queue
typedef void *Data; // opaque data item

extern Deq deq_new(); // constructor for the double linked list
extern int deq_len(Deq q); // number of items in the list

extern void deq_head_put(Deq q, Data d); // prepend to head
extern Data deq_head_get(Deq q); // remove and return from head
extern Data deq_head_ith(Deq q, int i); // return ith from head. return 0 if out of bounds.
extern Data deq_head_rem(Deq q, Data d); // remove and return d from head. if not found, return 0. If multiple found, remove first from head.

extern void deq_tail_put(Deq q, Data d); // append to tail
extern Data deq_tail_get(Deq q); // remove and return from tail
extern Data deq_tail_ith(Deq q, int i); // return ith from tail. return 0 if out of bounds.
extern Data deq_tail_rem(Deq q, Data d); // remove and return d from tail. if not found, return 0. If multiple found, remove first from tail.

typedef char *Str;
typedef void (*DeqMapF)(Data d);
typedef Str  (*DeqStrF)(Data d); // pointer to function that converts Data to string

extern void deq_map(Deq q, DeqMapF f); // foreach
extern void deq_del(Deq q, DeqMapF f); // free
extern Str  deq_str(Deq q, DeqStrF f); // toString

#endif
