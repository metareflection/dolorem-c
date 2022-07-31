#include "lower.h"
#include "cgen.h"
#include "hashmap.h"
#include "list.h"

#include <assert.h>
#include <dlfcn.h>
#include <libtcc.h>
#include <stdlib.h>
#include <string.h>

static struct StringListContainer header;
static map_t macros;
void *dlhdl;

void init_lower() {
  macros = hashmap_new();
  lower_mark_as_macro("progn");
  lower_mark_as_macro("include");
  lower_mark_as_macro("scope");
  lower_mark_as_macro("mark_as_macro");
  lower_mark_as_macro("function");
  lower_mark_as_macro("compile");
  lower_mark_as_macro("funproto");
  dlhdl = dlopen(NULL, RTLD_LAZY);
}
void end_lower() {}

static struct cexp *lower_call(struct val *e) {
  const char *name = sanitizeName(expect_ident(car(e)));
  void *dummy;
  if (hashmap_get(macros, name, &dummy) == MAP_OK) {
    /* call macro */
    return call_macro(name, cdr(e));
  } else {
    /* regular function call */
    struct cexp *r = make_cexp(print_to_mem("%s(", name), NULL, NULL, NULL);
    int first_element = 1;
    for (struct val *args = cdr(e); !is_nil(args); args = cdr(args)) {
      struct cexp *a = lower(car(args));
      add_cexp(r, a);
      r->E = print_to_mem(first_element ? "%s%s" : "%s, %s", r->E, a->E);
      first_element = 0;
    }
    r->E = print_to_mem("%s)", r->E);
    return r;
  }
}

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

struct cexp *progn(struct val *e) {
  struct cexp *r = make_cexp("", NULL, NULL, NULL);
  for (struct val *args = e; !is_nil(args); args = cdr(args)) {
    struct cexp *a = lower(car(args));
    add_cexp(r, a);
    if (a->E && *a->E && !is_nil(cdr(args))) {
      appendString(&r->Context, print_to_mem("%s;\n", a->E));
    }
    r->E = a->E;
  }
  return r;
}
struct cexp *compile_each(struct val *e) {
  for (struct val *args = e; !is_nil(args); args = cdr(args)) {
    lower_compile(lower(car(args)));
  }
  return make_cexp("", NULL, NULL, NULL);
}

const char *expect_type(struct val *e) {
  if (e->T == tyIdent) {
    return e->V.S;
  } else if (e->T == tyString) {
    return e->V.S;
  } else {
    compiler_error(e, "expected type");
  }
}

char *get_funproto(struct val *e) {
  const char *name = sanitizeName(expect_ident(car(e)));
  const char *ret = expect_type(car(cdr(cdr(e))));
  char *res = print_to_mem("%s %s(", ret, name);
  int first_element = 1;
  for (struct val *args = car(cdr((e))); !is_nil(args); args = cdr(args)) {
    struct val *a = car(args);
    if (a->T == tyIdent && !strcmp(a->V.S, "...")) {
      res = print_to_mem("%s, ...", res);
    } else {
      res = print_to_mem(first_element ? "%s%s %s" : "%s, %s %s", res,
                         expect_type(car(a)),
                         sanitizeName(expect_ident(car(cdr(a)))));
    }
    first_element = 0;
  }
  return res;
}
struct cexp *funproto(struct val *e) {
  return make_cexp("", NULL, NULL, print_to_mem("%s);", get_funproto(e)));
}
struct cexp *function(struct val *e) {
  const char *proto = get_funproto(e);
  struct cexp *body = lower(car(cdr(cdr(cdr(e)))));
  struct cexp *r =
      make_cexp("", NULL, NULL, print_to_mem("%s);", get_funproto(e)));
  appendStringList(&r->Header, body->Header);
  appendStringList(&r->Global, body->Global);
  appendString(&r->Global, proto);
  appendString(&r->Global, ") {\n");
  appendStringList(&r->Global, body->Context);
  if (strcmp(expect_ident(car(cdr(cdr(e)))), "void")) {
    appendString(&r->Global, "return ");
  }
  appendString(&r->Global, body->E);
  appendString(&r->Global, ";\n}\n");
  return r;
}

void lower_compile(struct cexp *c) {
  appendStringList(&header, c->Header);
  printf("compiling the following: [%s|%s|%s|%s]\n", c->E,
         toOneString(c->Context), toOneString(c->Global),
         toOneString(c->Header));

  if (c->Global.First) {
    char file[] = "/tmp/doloutXXXXXX";
    assert(mkstemp(file));
    TCCState *state = tcc_new();
    tcc_set_output_type(state, TCC_OUTPUT_DLL);
    tcc_compile_string(state, twoStringListsToOneString(header, c->Global));
    tcc_output_file(state, file);
    tcc_delete(state);

    if (!dlopen(file, RTLD_GLOBAL | RTLD_NOW)) {
      compiler_error_internal("linking failure: %s", dlerror());
    }
  }
}
struct cexp *compile(struct val *c) {
  lower_compile(lower(car(c)));
  return make_cexp("", NULL, NULL, NULL);
}

struct cexp *mark_as_macro(struct val *e) {
  lower_mark_as_macro(expect_ident(car(e)));
  return make_cexp("", NULL, NULL, NULL);
}
void lower_mark_as_macro(const char *e) {
  hashmap_put(macros, sanitizeName(e), NULL);
}

struct cexp *call_macro(const char *name, struct val *e) {
  struct cexp *(*m)(struct val *) =
      (struct cexp * (*)(struct val *)) dlsym(dlhdl, name);
  if (!m) {
    compiler_error(e, "function not found: \"%s\"", name);
  }
  return m(e);
}
