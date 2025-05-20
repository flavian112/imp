#ifndef INTERPRETER_H
#define INTERPRETER_H


#include "ast.h"
#include "hash_map.h"


void exec_stmt(hashmap_t context, ASTNode *node);
void context_print(hashmap_t context);


#endif