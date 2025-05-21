#include "repl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "interpreter.h"


static void print_help(void) {
  printf(
    "IMP REPL (type IMP statements or commands starting with '%%')\n"
    "Commands:\n"
    "  %%quit                   exit\n"
    "  %%run <path/to/file.imp> interpret program\n"
    "  %%set <var> <val>        set variable\n"
    "  %%print [<var>]          print variable, or all variables\n"
    "  %%help                   show this message\n");
}

static void repl_exec_command(context_t context, char *command) {
  char *cmd = strtok(command, " \t");
  if (strcmp(cmd, "%quit") == 0) {
    exit(EXIT_SUCCESS);
  } else if (strcmp(cmd, "%help") == 0) {
    print_help();
  } else if (strcmp(cmd, "%run") == 0) {
    char *file = strtok(NULL, " \t");
    if (file) {
      if (!interp_file(context, file)) context_print_var_table(context);
    } else {
      fprintf(stderr, "Usage: %%run <path/to/file.imp>\n");
    }
  } else if (strcmp(cmd, "%set") == 0) {
    char *var = strtok(NULL, " \t");
    char *val = strtok(NULL, " \t");
    if (var && val) context_set_var(context, var, atoi(val));
    else fprintf(stderr, "Usage: %%set <var> <val>\n");
  } else if (strcmp(cmd, "%print") == 0) {
    char *var = strtok(NULL, " \t");
    if (var) printf("%s = %d\n", var, context_get_var(context, var));
    else context_print_var_table(context);
  } else {
    fprintf(stderr, "Unknown command: %s\n", cmd);
  }
}

static void repl_exec_statement(context_t context, const char *statement) {
  if (!interp_str(context, statement)) context_print_var_table(context);
}

void repl(void) {
  context_t context = context_create();

  char *line;
  print_help();
  while ((line = readline("imp> ")) != NULL) {
    if (*line) add_history(line);
    if (line[0] == '%') repl_exec_command(context, line);
    else if (*line) repl_exec_statement(context, line);
    free(line);
  }
  printf("\n");

  context_free(context);
}