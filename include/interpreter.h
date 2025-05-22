#ifndef INTERPRETER_H
#define INTERPRETER_H


#include "ast.h"


typedef struct Context *context_t;


context_t context_create(void);
void context_free(context_t context);

int context_get_var(context_t context, const char *name);
void context_set_var(context_t context, const char *name, int value);
void context_print_var_table(context_t context);
void context_print_proc_table(context_t context);

int interp_ast(context_t context, ASTNode *node);
int interp_file (context_t context, const char *path);
int interp_str (context_t context, const char *str);

int print_ast_file (const char *path);


#endif