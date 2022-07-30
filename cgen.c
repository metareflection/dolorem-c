#include "cgen.h"
#include "list.h"

#include <string.h>

char nullStr;

void appendString(struct StringListContainer *sl, const char *s) {
  if (sl->Last) {
    sl->Last->L = get_mem(sizeof(struct StringList));
    sl->Last->L->S = s;
    sl->Last = sl->Last->L;
  } else {
    sl->Last = get_mem(sizeof(struct StringList));
    sl->Last->S = s;
    sl->First = sl->Last;
  }
}
void appendStringList(struct StringListContainer *sl,
                      struct StringListContainer l) {
  if (sl->First == l.First || !l.First) {
    return;
  }
  if (sl->Last) {
    sl->Last->L = l.First;
    sl->Last = l.Last;
  } else {
    *sl = l;
  }
}
void printAll(struct StringListContainer c) {
  struct StringList *sl;
  for (sl = c.First; sl; sl = sl->L) {
    puts(sl->S);
  }
}
char *toOneString(struct StringListContainer c) {
  struct StringList *s;
  char *r;
  unsigned len, i, l;
  len = 0;
  for (s = c.First; s; s = s->L) {
    len += strlen(s->S);
  }
  r = get_mem(len + 1);
  r[len] = 0;
  i = 0;
  for (s = c.First; s; s = s->L) {
    l = strlen(s->S);
    memcpy(r + i, s->S, l);
    i += l;
  }
  return r;
}
char *twoStringListsToOneString(struct StringListContainer c,
                                struct StringListContainer c2) {
  struct StringList *s;
  char *r;
  unsigned len, i, l;
  len = 0;
  for (s = c.First; s; s = s->L) {
    len += strlen(s->S);
  }
  for (s = c2.First; s; s = s->L) {
    len += strlen(s->S);
  }
  r = get_mem(len + 1);
  r[len] = 0;
  i = 0;
  for (s = c.First; s; s = s->L) {
    l = strlen(s->S);
    memcpy(r + i, s->S, l);
    i += l;
  }
  for (s = c2.First; s; s = s->L) {
    l = strlen(s->S);
    memcpy(r + i, s->S, l);
    i += l;
  }
  return r;
}
const char *sanitizeName(const char *name) {
  unsigned long len;
  char *r, *p;
  len = strlen(name);
  r = get_mem(len + 1);
  memcpy(r, name, len + 1);
  p = r;
  while (*p) {
    if (!((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') ||
          (*p >= '0' && *p <= '9'))) {
      *p = '_';
    }
    ++p;
  }
  return r;
}

int uniqueIntAt;
int uniqueInt() { return ++uniqueIntAt; }

struct cexp *make_cexp(const char *a, const char *before, const char *global,
                       const char *header) {
  struct cexp *r;
  r = get_mem(sizeof(struct cexp));
  r->E = a;
  if (before) {
    r->Context.Last = get_mem(sizeof(struct StringList));
    r->Context.Last->S = before;
    r->Context.First = r->Context.Last;
  }
  if (global) {
    r->Global.Last = get_mem(sizeof(struct StringList));
    r->Global.Last->S = global;
    r->Global.First = r->Global.Last;
  }
  if (header) {
    r->Header.Last = get_mem(sizeof(struct StringList));
    r->Header.Last->S = header;
    r->Header.First = r->Header.Last;
  }
  return r;
}
void add_cexp(struct cexp *a, struct cexp *b) {
  appendStringList(&a->Context, b->Context);
  appendStringList(&a->Global, b->Global);
  appendStringList(&a->Header, b->Header);
}
