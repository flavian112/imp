#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "interpreter.h"
#include "repl.h"

static void interpret_file(const char *path) {
  hashmap_t context = hashmap_create();
  exec_file(context, path);
  context_print(context);
  hashmap_free(context);
}

int main(int argc, char **argv) {
  int opt;
  const char *script = NULL;
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