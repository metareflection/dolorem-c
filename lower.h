#ifndef LOWER_H
#define LOWER_H

#include "cgen.h"
#include "list.h"

void init_lower();
void end_lower();

typedef struct cexp *(*macrofunptr)(struct val *);

struct cexp *lower(struct val *e);
struct cexp *progn(struct val *e);
struct cexp *compile_each(struct val *e);
struct cexp *function(struct val *e);
struct cexp *funproto(struct val *e);
struct cexp *compile(struct val *e);
void lower_compile(struct cexp *e);
struct cexp *mark_as_macro(struct val *e);
void lower_mark_as_macro(const char *e);
struct cexp *call_macro(const char *name, struct val *e);
macrofunptr override_macro(const char *macro, const char *newfun);
const char *expect_type(struct val *e);

#endif
