#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "interpreter.h"
#include "repl.h"


static int interpret_file(const char *path) {
  context_t context = context_create();
  if (interp_file(context, path)) {
    fprintf(stderr, "Error interpreting file: %s\n", path);
    context_free(context);
    return -1;
  }
  context_print_var_table(context);
  context_free(context);
  return 0;
}

int main(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "i:h")) != -1) {
    switch (opt){
    case 'i': 
      interpret_file(optarg);
      return EXIT_SUCCESS;
    case 'h':
    default:
      fprintf(stderr, 
        "Usage: %s [-i program.imp]\n"
        "  -i <program.imp>   interpret program and exit\n"
        "  (no args)          start REPL\n",
        argv[0]);
      return (opt == 'h') ? EXIT_SUCCESS : EXIT_FAILURE;
    }
  }
  repl();
  return EXIT_SUCCESS;
}