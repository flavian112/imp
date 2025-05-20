#ifndef INTERPRETER_H
#define INTERPRETER_H


#include "ast.h"
#include "hash_map.h"


void exec_stmt(hashmap_t context, ASTNode *node);
void context_print(hashmap_t context);

void context_set(hashmap_t context, const char *name, int value);
int context_get(hashmap_t context, const char *name);

int exec_file (hashmap_t context, const char *path);
int exec_str (hashmap_t context, const char *str);


#endif