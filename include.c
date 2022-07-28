#include "include.h"

#include "lower.h"

void init_include() { lower_mark_as_macro("include"); }
void end_include() {}
struct cexp *include(struct val *l) {
  struct val *name;
  name = car(l);
  if (!is_nil(cdr(l))) {
    compiler_error(cdr(l), "excess elements in \"include\"");
  }
  if (name->T != tyString) {
    compiler_error(name, "expected string");
  }
  return lower_include(name->V.S);
}
struct cexp *lower_include_list(const char *filename, struct val *list) {
  (void)filename;
  return progn(list);
}
struct cexp *lower_include(const char *filename) {
  return lower_include_list(filename, read_file(filename));
}
