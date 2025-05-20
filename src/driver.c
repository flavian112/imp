#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"

extern FILE *yyin;
extern int yyparse(void);
extern ASTNode *ast_root;

int main(int argc, char **argv) {
  if (argc > 1) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
      perror(argv[1]);
      return EXIT_FAILURE;
    }
  } else {
    yyin = stdin;
  }
  
  if (yyparse() != 0) {
    fprintf(stderr, "Parsing failed.\n");
    return EXIT_FAILURE;
  }
  
  hashmap_t context = hashmap_create();
  exec_stmt(context, ast_root);
  context_print(context);
  hashmap_free(context);
  ast_free(ast_root);
  
  return EXIT_SUCCESS;
}
