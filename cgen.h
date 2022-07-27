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

extern char nullStr;
