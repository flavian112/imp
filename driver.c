#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

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
  
  Env *env = NULL;
  exec_stmt(&env, ast_root);
  env_print(env);
  free_ast(ast_root);
  
  return EXIT_SUCCESS;
}
