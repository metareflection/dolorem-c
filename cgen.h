#ifndef CGEN_H
#define CGEN_H

#include "list.h"

typedef struct cv cv;

struct StringList {
  const char *S;
  struct StringList *L;
};
struct StringListContainer {
  struct StringList *First;
  struct StringList *Last;
};
void appendString(struct StringListContainer *sl, const char *s);
void appendStringList(struct StringListContainer *sl,
                      struct StringListContainer l);
void printAll(struct StringListContainer c);
char *toOneString(struct StringListContainer c);
char *twoStringListsToOneString(struct StringListContainer c1,
                                struct StringListContainer c2);
const char *sanitizeName(const char *name);

extern int uniqueIntAt;
int uniqueInt();

struct cexp {
  const char *E;
  struct StringListContainer Context;
  struct StringListContainer Global;
  struct StringListContainer Header;
};
struct cexp *make_cexp(const char *a, const char *before, const char *global,
                       const char *header);
void add_cexp(struct cexp *orig, struct cexp *add);
void append_cexp(struct cexp *orig, const char *a, const char *before,
                 const char *global, const char *header);
const char *get_expression(struct cexp *e);
const char *get_context(struct cexp *e);
const char *get_global(struct cexp *e);
const char *get_header(struct cexp *e);

extern char nullStr;

#endif
