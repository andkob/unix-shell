#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deq.h"
#include "error.h"

// indices and size of array of node pointers
typedef enum
{
  Head,
  Tail,
  Ends
} End;

typedef struct Node
{
  struct Node *np[Ends]; // next/prev neighbors (Ends=2 (2 elements))
  Data data;
} *Node; // a var of type Node is a pointer to a struct Node (no need to include the * when declaring)

typedef struct
{
  Node ht[Ends]; // head/tail nodes
  int len;
} *Rep; // "Rep = representation"

static Rep rep(Deq q)
{
  if (!q)
    ERROR("zero pointer");
  return (Rep)q;
}

static void put(Rep r, End e, Data d)
{
  Node n = malloc(sizeof(*n));
  if (!n)
    ERROR("malloc() failed");
  n->data = d;

  if (r->len == 0)
  {
    // empty deque
    n->np[Head] = n->np[Tail] = NULL;
    r->ht[Head] = r->ht[Tail] = n;
  }
  else if (e == Head)
  {
    // insert at head
    n->np[Tail] = r->ht[Head];
    n->np[Head] = NULL;
    r->ht[Head]->np[Head] = n;
    r->ht[Head] = n;
  }
  else
  {
    // insert at tail
    n->np[Head] = r->ht[Tail];
    n->np[Tail] = NULL;
    r->ht[Tail]->np[Tail] = n;
    r->ht[Tail] = n;
  }
  r->len++;
}

static Data ith(Rep r, End e, int i)
{
  if (i < 0 || i >= r->len)
    return 0;

  Node n = r->ht[e];
  for (int j = 0; j < i; j++)
  {
    n = n->np[(e == Head) ? Tail : Head];
  }
  return n->data;
}

static Data get(Rep r, End e)
{
  if (r->len == 0)
    return 0;

  Node n = r->ht[e];
  Data d = n->data;

  if (r->len == 1)
  {
    r->ht[Head] = r->ht[Tail] = NULL;
  }
  else if (e == Head)
  {
    r->ht[Head] = n->np[Tail];
    r->ht[Head]->np[Head] = NULL;
  }
  else
  {
    r->ht[Tail] = n->np[Head];
    r->ht[Tail]->np[Tail] = NULL;
  }
  free(n);
  r->len--;
  return d;
}

static Data rem(Rep r, End e, Data d)
{
  Node n = r->ht[e];
  while (n)
  {
    if (n->data == d)
    {
      if (n->np[Head])
        n->np[Head]->np[Tail] = n->np[Tail];
      else
        r->ht[Head] = n->np[Tail];

      if (n->np[Tail])
        n->np[Tail]->np[Head] = n->np[Head];
      else
        r->ht[Tail] = n->np[Head];

      Data data = n->data;
      free(n);
      r->len--;
      return data;
    }
    n = n->np[(e == Head) ? Tail : Head];
  }
  return 0;
}

extern Deq deq_new()
{
  Rep r = (Rep)malloc(sizeof(*r));
  if (!r)
    ERROR("malloc() failed");
  r->ht[Head] = 0;
  r->ht[Tail] = 0;
  r->len = 0;
  return r;
}

extern int deq_len(Deq q) { return rep(q)->len; }

extern void deq_head_put(Deq q, Data d) { put(rep(q), Head, d); }
extern Data deq_head_get(Deq q) { return get(rep(q), Head); }
extern Data deq_head_ith(Deq q, int i) { return ith(rep(q), Head, i); }
extern Data deq_head_rem(Deq q, Data d) { return rem(rep(q), Head, d); }

extern void deq_tail_put(Deq q, Data d) { put(rep(q), Tail, d); }
extern Data deq_tail_get(Deq q) { return get(rep(q), Tail); }
extern Data deq_tail_ith(Deq q, int i) { return ith(rep(q), Tail, i); }
extern Data deq_tail_rem(Deq q, Data d) { return rem(rep(q), Tail, d); }

// Foreach: apply function f to each data item from head to tail
extern void deq_map(Deq q, DeqMapF f)
{
  for (Node n = rep(q)->ht[Head]; n; n = n->np[Tail])
    f(n->data);
}

// Descructor: free all nodes, then the Deq itself
extern void deq_del(Deq q, DeqMapF f)
{
  if (f)
    deq_map(q, f);
  Node curr = rep(q)->ht[Head];
  while (curr)
  {
    Node next = curr->np[Tail];
    free(curr);
    curr = next;
  }
  free(q);
}

extern Str deq_str(Deq q, DeqStrF f)
{
  char *s = strdup("");
  for (Node n = rep(q)->ht[Head]; n; n = n->np[Tail])
  {
    char *d = f ? f(n->data) : n->data;
    char *t;
    asprintf(&t, "%s%s%s", s, (*s ? " " : ""), d);
    free(s);
    s = t;
    if (f)
      free(d);
  }
  return s;
}
