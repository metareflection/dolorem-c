#ifndef INCLUDE_H
#define INCLUDE_H

#include "cgen.h"
#include "list.h"

void init_include();
void end_include();
struct cexp *include(struct val *e);
struct cexp *lower_include(const char *name);
struct cexp *lower_include_list(const char *filename, struct val *list);

#endif
