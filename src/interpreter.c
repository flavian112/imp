#include "interpreter.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "hashmap.h"


typedef void *YY_BUFFER_STATE;
extern FILE *yyin;
extern ASTNode *ast_root;
extern int yyparse(void);
extern void yyrestart (FILE*);
extern YY_BUFFER_STATE yy_scan_string(const char*);
extern void yy_delete_buffer(YY_BUFFER_STATE);

typedef struct Context{
  hashmap_t var_table;
  hashmap_t proc_table;
} Context;


static ASTNode *context_get_proc(Context *context, const char *name) {
  ASTNode **procdecl = (ASTNode**)hashmap_get(context->proc_table, name);
  if (procdecl) return *procdecl;
  return NULL;
}

static void context_set_proc(Context *context, const char *name, ASTNode *procdecl) {
  hashmap_insert(context->proc_table, name, (void*)procdecl);
}

Context *context_create(void) {
  Context *context = malloc(sizeof(Context));
  assert(context);
  context->var_table = hashmap_create();
  context->proc_table = hashmap_create();
  return context;
}

void context_free(Context *context) {
  hashmap_free(context->var_table);
  hashmap_keys_iter_t iter = hashmap_keys_iter_create(context->proc_table);
  const char *key;
  while ((key = hashmap_keys_iter_next(iter)) != NULL) {
    ASTNode *procdecl = context_get_proc(context, key);
    assert(procdecl);
    ast_free(procdecl);
  }
  hashmap_keys_iter_free(iter);
  hashmap_free(context->proc_table);
  free(context);
}

int context_get_var(Context *context, const char *name) {
  int *val = (int*)hashmap_get(context->var_table, name);
  if (val) return *val;
  return 0;
}

void context_set_var(Context *context, const char *name, int val) {
  hashmap_insert(context->var_table, name, (void*)val);
}

void context_print_var_table(Context *context) {
  hashmap_keys_iter_t iter = hashmap_keys_iter_create(context->var_table);
  const char *key;
  while ((key = hashmap_keys_iter_next(iter)) != NULL) {
    int val = context_get_var(context, key);
    printf("%s = %d\n", key, val);
  }
  hashmap_keys_iter_free(iter);
}

void context_print_proc_table(Context *context) {
  hashmap_keys_iter_t iter = hashmap_keys_iter_create(context->proc_table);
  const char *key;
  while ((key = hashmap_keys_iter_next(iter)) != NULL) {
    ASTNode *procdecl = context_get_proc(context, key);
    printf("%s(", key);
    ASTNodeList *args = procdecl->u.d_procdecl.args;
    while (args) {
      printf("%s", args->node->u.d_var.name);
      args = args->next;
      if (args) printf(", ");
    }
    printf("; ");
    ASTNodeList *vargs = procdecl->u.d_procdecl.vargs;
    while (vargs) {
      printf("%s", vargs->node->u.d_var.name);
      vargs = vargs->next;
      if (vargs) printf(", ");
    }
  }
  printf(")\n");
  hashmap_keys_iter_free(iter);
}

static int eval_aexpr(Context *context, ASTNode *node) {
  switch (node->type) {
    case NT_INT: return node->u.d_int.val;
    case NT_VAR: return context_get_var(context, node->u.d_var.name);
    case NT_AOP: {
      int aexp1_val = eval_aexpr(context, node->u.d_aop.aexp1);
      int aexp2_val = eval_aexpr(context, node->u.d_aop.aexp2);
      switch (node->u.d_aop.aop) {
        case AOP_ADD: return aexp1_val + aexp2_val;
        case AOP_SUB: return aexp1_val - aexp2_val;
        case AOP_MUL: return aexp1_val * aexp2_val;
        default: assert(0);
      }
    }
    default: assert(0);
  }
}

static int eval_bexpr(Context *context, ASTNode *node) {
  switch (node->type) {
    case NT_BOP: {
      int bexp1_val = eval_bexpr(context, node->u.d_bop.bexp1);
      int bexp2_val = eval_bexpr(context, node->u.d_bop.bexp2);
      switch (node->u.d_bop.bop) {
        case BOP_AND: return bexp1_val && bexp2_val;
        case BOP_OR:  return bexp1_val || bexp2_val;
        default: assert(0);
      }
    }
    case NT_NOT: return !eval_bexpr(context, node->u.d_not.bexp);
    case NT_ROP: {
      int aexp1_val = eval_aexpr(context, node->u.d_rop.aexp1);
      int aexp2_val = eval_aexpr(context, node->u.d_rop.aexp2);
      switch (node->u.d_rop.rop) {
        case ROP_EQ: return aexp1_val == aexp2_val;
        case ROP_NE: return aexp1_val != aexp2_val;
        case ROP_LT: return aexp1_val < aexp2_val;
        case ROP_LE: return aexp1_val <= aexp2_val;
        case ROP_GT: return aexp1_val > aexp2_val;
        case ROP_GE: return aexp1_val >= aexp2_val;
        default: assert(0);
      }
    }
    default: assert(0);
  }
}

static int interp_proccall(Context *context, ASTNode *node) {
  char *name = node->u.d_proccall.name;
  ASTNode *procdecl = context_get_proc(context, name);
  if (!procdecl) {
    fprintf(stderr, "Error: procedure %s not defined\n", name);
    return -1;
  }
  ASTNodeList *caller_args = node->u.d_proccall.args;
  ASTNodeList *callee_args = procdecl->u.d_procdecl.args;
  Context *proc_context = context_create();
  hashmap_keys_iter_t iter = hashmap_keys_iter_create(context->proc_table);
  const char *key;
  while ((key = hashmap_keys_iter_next(iter)) != NULL) {
    ASTNode *proc = context_get_proc(context, key);
    context_set_proc(proc_context, key, ast_clone(proc));
  }
  hashmap_keys_iter_free(iter);
  while (caller_args && callee_args) {
    char *caller_arg_name = caller_args->node->u.d_var.name;
    char *callee_arg_name = callee_args->node->u.d_var.name;
    context_set_var(proc_context, callee_arg_name, context_get_var(context, caller_arg_name));
    caller_args = caller_args->next;
    callee_args = callee_args->next;
  }
  if (caller_args || callee_args) {
    fprintf(stderr, "Error: procedure %s called with wrong number of value arguments\n", name);
    context_free(proc_context);
    return -1;
  }
  if (interp_ast(proc_context, procdecl->u.d_procdecl.stm)) {
    context_free(proc_context);
    return -1;
  }
  ASTNodeList *caller_vargs = node->u.d_proccall.vargs;
  ASTNodeList *callee_vargs = procdecl->u.d_procdecl.vargs;
  while (caller_vargs && callee_vargs) {
    char *caller_varg_name = caller_vargs->node->u.d_var.name;
    char *callee_varg_name = callee_vargs->node->u.d_var.name;
    context_set_var(context, caller_varg_name, context_get_var(proc_context, callee_varg_name));
    caller_vargs = caller_vargs->next;
    callee_vargs = callee_vargs->next;
  }
  if (caller_vargs || callee_vargs) {
    fprintf(stderr, "Error: procedure %s called with wrong number of variable arguments\n", name);
    context_free(proc_context);
    return -1;
  }
  context_free(proc_context);
  return 0;
}

int interp_ast(Context *context, ASTNode *node) {
  switch (node->type) {
    case NT_SKIP: return 0;
    case NT_ASSIGN: {
      char *name = node->u.d_assign.var->u.d_var.name;
      int val = eval_aexpr(context, node->u.d_assign.aexp);
      context_set_var(context, name, val);
      return 0;
    }
    case NT_SEQ:
      if (interp_ast(context, node->u.d_seq.stm1)) return -1;
      if (interp_ast(context, node->u.d_seq.stm2)) return -1;
      return 0;
    case NT_IF:
      if (eval_bexpr(context, node->u.d_if.bexp)) return interp_ast(context, node->u.d_if.stm1);
      else return interp_ast(context, node->u.d_if.stm2);
    case NT_WHILE:
      while (eval_bexpr(context, node->u.d_while.bexp)) {
        if (interp_ast(context, node->u.d_while.stm)) return -1;
      }
      return 0;
    case NT_LET: {
      char *name = node->u.d_let.var->u.d_var.name;
      int old_val = context_get_var(context, name);
      int new_val = eval_aexpr(context, node->u.d_let.aexp);
      context_set_var(context, name, new_val);
      int ret = interp_ast(context, node->u.d_let.stm);
      context_set_var(context, name, old_val);
      return ret;
    }
    case NT_PROCDECL: {
      char *name = node->u.d_procdecl.name;
      ASTNode *procdecl_old = context_get_proc(context, name);
      if (procdecl_old) {
        fprintf(stderr, "Error: procedure %s already defined\n", name);
        return -1;
      }
      ASTNode *procdecl = ast_clone(node);
      context_set_proc(context, name, procdecl);
      return 0;
    }
    case NT_PROCCALL: {
      return interp_proccall(context, node);
    }
    default: assert(0);
  }
}

int interp_file (Context *context, const char *path) {
  yyin = fopen(path, "r");
  if (!yyin) {
    return -1;
  }
  yyrestart(yyin);
  if (yyparse()) {
    ast_free(ast_root);
    fclose(yyin);
    return -1;
  }
  if (interp_ast(context, ast_root)) {
    ast_free(ast_root);
    fclose(yyin);
    return -1;
  }
  ast_free(ast_root);
  fclose(yyin);
  return 0;
}

int interp_str (Context *context, const char *str) {
  YY_BUFFER_STATE buf = yy_scan_string(str);
  if (yyparse()) {
    ast_free(ast_root);
    yy_delete_buffer(buf);
    return -1;
  }
  if (interp_ast(context, ast_root)) {
    ast_free(ast_root);
    yy_delete_buffer(buf);
    return -1;
  }
  yy_delete_buffer(buf);
  return 0;
}