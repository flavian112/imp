#include "repl.h"
#include "interpreter.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_help(void) {
  puts(
    "IMP REPL (type IMP statements or commands starting with '%')\n"
    "Commands:\n"
    "  %quit                   exit\n"
    "  %run <path/to/file.imp> interpret program\n"
    "  %set <var> <val>        set variable\n"
    "  %print [<var>]          print variable, or all variables\n"
    "  %help                   show this message\n");
}

static void repl_exec_command(hashmap_t context, const char *command) {
  char *cmd = strtok(command, " \t");
  if (strcmp(cmd, "%quit") == 0) {
    exit(EXIT_SUCCESS);
  } else if (strcmp(cmd, "%help") == 0) {
    print_help();
  } else if (strcmp(cmd, "%run") == 0) {
    char *file = strtok(NULL, " \t");
    if (file) {
      if (!exec_file(context, file)) context_print(context);
    } else {
      fprintf(stderr, "Usage: %%run <path/to/file.imp>\n");
    }
  } else if (strcmp(cmd, "%set") == 0) {
    char *var = strtok(NULL, " \t");
    char *val = strtok(NULL, " \t");
    if (var && val) context_set(context, var, atoi(val));
    else fprintf(stderr, "Usage: %%set <var> <val>\n");
  } else if (strcmp(cmd, "%print") == 0) {
    char *var = strtok(NULL, " \t");
    if (var) printf("%s = %d\n", var, context_get(context, var));
    else context_print(context);
  } else {
    fprintf(stderr, "Unknown command: %s\n", cmd);
  }
}

static void repl_exec_statement(hashmap_t context, const char *statement) {
  if (!exec_str(context, statement)) context_print(context);
}

void repl(void) {
  hashmap_t context = hashmap_create();

  char *line;
  print_help();
  while ((line = readline("imp> ")) != NULL) {
    if (*line) add_history(line);
    if (line[0] == '%') repl_exec_command(context, line);
    else if (*line) repl_exec_statement(context, line);
    free(line);
  }
  printf("\n");

  hashmap_free(context);
}