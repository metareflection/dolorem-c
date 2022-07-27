#ifndef LOWER_H
#define LOWER_H

#include "cgen.h"
#include "list.h"

struct cexp *lower(struct val *e);
struct cexp *progn(struct val *e);
struct cexp *scope(struct val *e);
struct cexp *function(struct val *e);
struct cexp *compile(struct val *e);
struct cexp *mark_as_macro(struct val *e);
void lower_mark_as_macro(const char *e);

#endif
