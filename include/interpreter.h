#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"


typedef struct Env {
  char *name;
  int val;
  struct Env *next;
} Env;

void exec_stmt(Env **env, ASTNode *node);
void env_print(Env *env);


#endif