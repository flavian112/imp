#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "interpreter_context.h"
#include "driver.h"
#include "repl.h"


static int interpret_file(const char *path) {
  IMP_InterpreterContext *context = imp_interpreter_context_create();
  if (imp_driver_interpret_file(context, path)) {
    fprintf(stderr, "Error interpreting file: %s\n", path);
    imp_interpreter_context_destroy(context);
    return -1;
  }
  imp_driver_print_var_table(context);
  imp_interpreter_context_destroy(context);
  return 0;
}

int main(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "i:a:h")) != -1) {
    switch (opt) {
    case 'i': 
      return interpret_file(optarg) ? EXIT_FAILURE : EXIT_SUCCESS;
    case 'a':
      return imp_driver_print_ast_file(optarg) ? EXIT_FAILURE : EXIT_SUCCESS;
    case 'h':
    default:
      fprintf(stderr, 
        "Usage: %s [ARGS]\n"
        "  (no args)          start REPL\n"
        "  -i <program.imp>   interpret program\n"
        "  -a <program.imp>   print ast\n"
        "  -h                 print this message\n",
        argv[0]);
      return (opt == 'h') ? EXIT_SUCCESS : EXIT_FAILURE;
    }
  }
  imp_repl();
  return EXIT_SUCCESS;
}