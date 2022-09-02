#include "cgen.h"
#include "include.h"
#include "list.h"
#include "lower.h"

#include <alloca.h>
#include <string.h>

int main(int argc, char **argv) {
  int i;
  int read_filename;
  struct val **files;
  read_filename = 0;
  files = alloca(argc * sizeof(char *));
  for (i = 1; i < argc; ++i) {
    files[i] = NULL;
    if (strcmp(argv[i], "-") == 0) {
      files[i] = read_stdin();
    } else if (strcmp(argv[i], "--dump-c-code") == 0 ||
               strcmp(argv[i], "-C") == 0) {
      cmdswitch_dump_c_code = 1;
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      printf("USAGE: %s [--dump-c-code] [--help] FILES...\n", argv[0]);
      return 0;
    } else {
      files[i] = read_file(argv[i]);
    }
  }

  init_alloc();
  init_lower();
  init_include();

  for (i = 1; i < argc; ++i) {
    if (files[i]) {
      progn(files[i]);
      read_filename = 1;
    }
  }
  if (!read_filename) {
    compiler_hint_internal("USAGE: %s [--dump-c-code] [--help] FILES...",
                           argv[0]);
    compiler_error_internal("no filename specified");
  }

  end_include();
  end_lower();
  end_alloc();
  return 0;
}
