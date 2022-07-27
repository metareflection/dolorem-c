#include "lower.h"
#include "hashmap.h"
#include "list.h"

#include <assert.h>

static struct StringListContainer header;
static map_t macros;

void init_lower() {
  lower_mark_as_macro("progn");
  lower_mark_as_macro("scope");
  lower_mark_as_macro("mark-as-macro");
  lower_mark_as_macro("lower");
  lower_mark_as_macro("function");
  lower_mark_as_macro("compile");
  macros = hashmap_new();
}
void end_lower() {}

struct cexp *lower(struct val *e) {
  switch (e->T) {
  case tyCons:
    return lower_call(e);
  case tyInt:
    return make_cexp(print_to_mem("%i", e->V.I), NULL, NULL, NULL);
  case tyFloat:
    return make_cexp(print_to_mem("%f", e->V.F), NULL, NULL, NULL);
  case tyIdent:
    return make_cexp(sanitizeName(e->V.S), NULL, NULL, NULL);
  case tyString:
    return make_cexp(print_to_mem("\"%s\"", e->V.S), NULL, NULL, NULL);
  default:
    assert(!"unknown type");
  }
}

static cexp *lower_call(struct val *e) { /* TODO */ }

struct cexp *progn(struct val *e);
struct cexp *scope(struct val *e);
struct cexp *function(struct val *e);
struct cexp *compile(struct val *e);
struct cexp *mark_as_macro(struct val *e) {
  lower_mark_as_macro(expect_ident(car(e)));
  return make_cexp("", NULL, NULL, NULL);
}
void lower_mark_as_macro(const char *e) { hashmap_put(macros, e, NULL); }
