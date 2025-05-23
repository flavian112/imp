#include "interpreter.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "hashmap.h"


typedef void *YY_BUFFER_STATE;
extern FILE *yyin;
extern IMP_ASTNode *ast_root;
extern int yyparse(void);
extern void yyrestart (FILE*);
extern YY_BUFFER_STATE yy_scan_string(const char*);
extern void yy_delete_buffer(YY_BUFFER_STATE);

typedef struct Context{
  hashmap_t var_table;
  hashmap_t proc_table;
} Context;


static IMP_ASTNode *context_get_proc(Context *context, const char *name) {
  IMP_ASTNode **procdecl = (IMP_ASTNode**)hashmap_get(context->proc_table, name);
  if (procdecl) return *procdecl;
  return NULL;
}

static void context_set_proc(Context *context, const char *name, IMP_ASTNode *procdecl) {
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
    IMP_ASTNode *procdecl = context_get_proc(context, key);
    assert(procdecl);
    imp_ast_free(procdecl);
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
    IMP_ASTNode *procdecl = context_get_proc(context, key);
    printf("%s(", key);
    IMP_ASTNodeList *args = procdecl->data.proc_decl.val_args;
    while (args) {
      printf("%s", args->node->data.variable.name);
      args = args->next;
      if (args) printf(", ");
    }
    printf("; ");
    IMP_ASTNodeList *vargs = procdecl->data.proc_decl.var_args;
    while (vargs) {
      printf("%s", vargs->node->data.variable.name);
      vargs = vargs->next;
      if (vargs) printf(", ");
    }
    printf(")\n");
  }
  hashmap_keys_iter_free(iter);
}

void ast_print(IMP_ASTNode *node, int depth) {
  int indent = depth * 2;
  switch (node->type) {
    case IMP_AST_NT_SKIP: {
      printf("%*sSKIP\n", indent, "");
      break;
    }
    case IMP_AST_NT_ASSIGN: {
      printf("%*sASSIGN %s=", indent, "", 
             node->data.assign.var->data.variable.name);
      ast_print(node->data.assign.aexpr, 0);
      printf("\n");
      break;
    }
    case IMP_AST_NT_SEQ: {
      ast_print(node->data.seq.fst_stmt, depth);
      printf("%*sSEQ\n", indent, "");
      ast_print(node->data.seq.snd_stmt, depth);
      break;
    }
    case IMP_AST_NT_IF: {
      printf("%*sIF (", indent, "");
      ast_print(node->data.if_stmt.cond_bexpr, 0);
      printf(")\n");
      ast_print(node->data.if_stmt.then_stmt, depth + 1);
      printf("%*sELSE\n", indent, "");
      ast_print(node->data.if_stmt.else_stmt, depth + 1);
      break;
    }
    case IMP_AST_NT_WHILE: {
      printf("%*sWHILE (", indent, "");
      ast_print(node->data.while_stmt.cond_bexpr, 0);
      printf(")\n");
      ast_print(node->data.while_stmt.body_stmt, depth + 1);
      break;
    }
    case IMP_AST_NT_INT: {
      printf("%d", node->data.integer.val);
      break;
    }
    case IMP_AST_NT_VAR: {
      printf("%s", node->data.variable.name);
      break;
    }
    case IMP_AST_NT_AOP: {
      printf("(");
      ast_print(node->data.arith_op.l_aexpr, 0);
      switch (node->data.arith_op.aopr) {
        case IMP_AST_AOP_ADD: printf(" + "); break;
        case IMP_AST_AOP_SUB: printf(" - "); break;
        case IMP_AST_AOP_MUL: printf(" * "); break;
        default: assert(0);
      }
      ast_print(node->data.arith_op.r_aexpr, 0);
      printf(")");
      break;
    }
    case IMP_AST_NT_BOP: {
      printf("(");
      ast_print(node->data.bool_op.l_bexpr, 0);
      switch (node->data.bool_op.bopr) {
        case IMP_AST_BOP_AND: printf(" && "); break;
        case IMP_AST_BOP_OR:  printf(" || "); break;
        default: assert(0);
      }
      ast_print(node->data.bool_op.r_bexpr, 0);
      printf(")");
      break;
    }
    case IMP_AST_NT_NOT: {
      printf("!");
      ast_print(node->data.bool_not.bexpr, 0);
      break;
    }
    case IMP_AST_NT_ROP: {
      printf("(");
      ast_print(node->data.rel_op.l_aexpr, 0);
      switch (node->data.rel_op.ropr) {
        case IMP_AST_ROP_EQ: printf(" == "); break;
        case IMP_AST_ROP_NE: printf(" != "); break;
        case IMP_AST_ROP_LT: printf(" < "); break;
        case IMP_AST_ROP_LE: printf(" <= "); break;
        case IMP_AST_ROP_GT: printf(" > "); break;
        case IMP_AST_ROP_GE: printf(" >= "); break;
        default: assert(0);
      }
      ast_print(node->data.rel_op.r_aexpr, 0);
      printf(")");
      break;
    }
    case IMP_AST_NT_LET: {
      printf("%*sLET %s = ", indent, "", node->data.let_stmt.var->data.variable.name);
      ast_print(node->data.let_stmt.aexpr, 0);
      printf("\n");
      ast_print(node->data.let_stmt.body_stmt, depth + 1);
      break;
    }
    case IMP_AST_NT_PROCDECL: {
      printf("%*sPROC %s(", indent, "", node->data.proc_decl.name);
      IMP_ASTNodeList *args = node->data.proc_decl.val_args;
      while (args) {
        printf("%s", args->node->data.variable.name);
        args = args->next;
        if (args) printf(", ");
      }
      printf("; ");
      IMP_ASTNodeList *vargs = node->data.proc_decl.var_args;
      while (vargs) {
        printf("%s", vargs->node->data.variable.name);
        vargs = vargs->next;
        if (vargs) printf(", ");
      }
      printf(")\n");
      ast_print(node->data.proc_decl.body_stmt, depth + 1);
      break;
    }
    case IMP_AST_NT_PROCCALL: {
      printf("%*sCALL %s(", indent, "", node->data.proc_call.name);
      IMP_ASTNodeList *args = node->data.proc_call.val_args;
      while (args) {
        printf("%s", args->node->data.variable.name);
        args = args->next;
        if (args) printf(", ");
      }
      printf("; ");
      IMP_ASTNodeList *vargs = node->data.proc_call.var_args;
      while (vargs) {
        printf("%s", vargs->node->data.variable.name);
        vargs = vargs->next;
        if (vargs) printf(", ");
      }
      printf(")\n");
      break;
    }
    default: assert(0);
  }
}

static int eval_aexpr(Context *context, IMP_ASTNode *node) {
  switch (node->type) {
    case IMP_AST_NT_INT: return node->data.integer.val;
    case IMP_AST_NT_VAR: return context_get_var(context, node->data.variable.name);
    case IMP_AST_NT_AOP: {
      int aexp1_val = eval_aexpr(context, node->data.arith_op.l_aexpr);
      int aexp2_val = eval_aexpr(context, node->data.arith_op.r_aexpr);
      switch (node->data.arith_op.aopr) {
        case IMP_AST_AOP_ADD: return aexp1_val + aexp2_val;
        case IMP_AST_AOP_SUB: return aexp1_val - aexp2_val;
        case IMP_AST_AOP_MUL: return aexp1_val * aexp2_val;
        default: assert(0);
      }
    }
    default: assert(0);
  }
}

static int eval_bexpr(Context *context, IMP_ASTNode *node) {
  switch (node->type) {
    case IMP_AST_NT_BOP: {
      int bexp1_val = eval_bexpr(context, node->data.bool_op.l_bexpr);
      int bexp2_val = eval_bexpr(context, node->data.bool_op.r_bexpr);
      switch (node->data.bool_op.bopr) {
        case IMP_AST_BOP_AND: return bexp1_val && bexp2_val;
        case IMP_AST_BOP_OR:  return bexp1_val || bexp2_val;
        default: assert(0);
      }
    }
    case IMP_AST_NT_NOT: return !eval_bexpr(context, node->data.bool_not.bexpr);
    case IMP_AST_NT_ROP: {
      int aexp1_val = eval_aexpr(context, node->data.rel_op.l_aexpr);
      int aexp2_val = eval_aexpr(context, node->data.rel_op.r_aexpr);
      switch (node->data.rel_op.ropr) {
        case IMP_AST_ROP_EQ: return aexp1_val == aexp2_val;
        case IMP_AST_ROP_NE: return aexp1_val != aexp2_val;
        case IMP_AST_ROP_LT: return aexp1_val < aexp2_val;
        case IMP_AST_ROP_LE: return aexp1_val <= aexp2_val;
        case IMP_AST_ROP_GT: return aexp1_val > aexp2_val;
        case IMP_AST_ROP_GE: return aexp1_val >= aexp2_val;
        default: assert(0);
      }
    }
    default: assert(0);
  }
}

static int interp_proccall(Context *context, IMP_ASTNode *node) {
  const char *name = node->data.proc_call.name;
  IMP_ASTNode *procdecl = context_get_proc(context, name);
  if (!procdecl) {
    fprintf(stderr, "Error: procedure %s not defined\n", name);
    return -1;
  }
  IMP_ASTNodeList *caller_args = node->data.proc_call.val_args;
  IMP_ASTNodeList *callee_args = procdecl->data.proc_decl.val_args;
  Context *proc_context = context_create();
  hashmap_keys_iter_t iter = hashmap_keys_iter_create(context->proc_table);
  const char *key;
  while ((key = hashmap_keys_iter_next(iter)) != NULL) {
    IMP_ASTNode *proc = context_get_proc(context, key);
    context_set_proc(proc_context, key, imp_ast_clone(proc));
  }
  hashmap_keys_iter_free(iter);
  while (caller_args && callee_args) {
    const char *caller_arg_name = caller_args->node->data.variable.name;
    const char *callee_arg_name = callee_args->node->data.variable.name;
    context_set_var(proc_context, callee_arg_name, context_get_var(context, caller_arg_name));
    caller_args = caller_args->next;
    callee_args = callee_args->next;
  }
  if (caller_args || callee_args) {
    fprintf(stderr, "Error: procedure %s called with wrong number of value arguments\n", name);
    context_free(proc_context);
    return -1;
  }
  if (interp_ast(proc_context, procdecl->data.proc_decl.body_stmt)) {
    context_free(proc_context);
    return -1;
  }
  IMP_ASTNodeList *caller_vargs = node->data.proc_call.var_args;
  IMP_ASTNodeList *callee_vargs = procdecl->data.proc_decl.var_args;
  while (caller_vargs && callee_vargs) {
    const char *caller_varg_name = caller_vargs->node->data.variable.name;
    const char *callee_varg_name = callee_vargs->node->data.variable.name;
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

int interp_ast(Context *context, IMP_ASTNode *node) {
  switch (node->type) {
    case IMP_AST_NT_SKIP: return 0;
    case IMP_AST_NT_ASSIGN: {
      const char *name = node->data.assign.var->data.variable.name;
      int val = eval_aexpr(context, node->data.assign.aexpr);
      context_set_var(context, name, val);
      return 0;
    }
    case IMP_AST_NT_SEQ:
      if (interp_ast(context, node->data.seq.fst_stmt)) return -1;
      if (interp_ast(context, node->data.seq.snd_stmt)) return -1;
      return 0;
    case IMP_AST_NT_IF:
      if (eval_bexpr(context, node->data.if_stmt.cond_bexpr)) return interp_ast(context, node->data.if_stmt.then_stmt);
      else return interp_ast(context, node->data.if_stmt.else_stmt);
    case IMP_AST_NT_WHILE:
      while (eval_bexpr(context, node->data.while_stmt.cond_bexpr)) {
        if (interp_ast(context, node->data.while_stmt.body_stmt)) return -1;
      }
      return 0;
    case IMP_AST_NT_LET: {
      const char *name = node->data.let_stmt.var->data.variable.name;
      int old_val = context_get_var(context, name);
      int new_val = eval_aexpr(context, node->data.let_stmt.aexpr);
      context_set_var(context, name, new_val);
      int ret = interp_ast(context, node->data.let_stmt.body_stmt);
      context_set_var(context, name, old_val);
      return ret;
    }
    case IMP_AST_NT_PROCDECL: {
      const char *name = node->data.proc_decl.name;
      IMP_ASTNode *procdecl_old = context_get_proc(context, name);
      if (procdecl_old) {
        fprintf(stderr, "Error: procedure %s already defined\n", name);
        return -1;
      }
      IMP_ASTNode *procdecl = imp_ast_clone(node);
      context_set_proc(context, name, procdecl);
      return 0;
    }
    case IMP_AST_NT_PROCCALL: {
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
    imp_ast_free(ast_root);
    fclose(yyin);
    return -1;
  }
  if (interp_ast(context, ast_root)) {
    imp_ast_free(ast_root);
    fclose(yyin);
    return -1;
  }
  imp_ast_free(ast_root);
  fclose(yyin);
  return 0;
}

int interp_str (Context *context, const char *str) {
  YY_BUFFER_STATE buf = yy_scan_string(str);
  if (yyparse()) {
    imp_ast_free(ast_root);
    yy_delete_buffer(buf);
    return -1;
  }
  if (interp_ast(context, ast_root)) {
    imp_ast_free(ast_root);
    yy_delete_buffer(buf);
    return -1;
  }
  yy_delete_buffer(buf);
  return 0;
}

int print_ast_file (const char *path) {
  yyin = fopen(path, "r");
  if (!yyin) {
    return -1;
  }
  yyrestart(yyin);
  if (yyparse()) {
    imp_ast_free(ast_root);
    fclose(yyin);
    return -1;
  }
  ast_print(ast_root, 0);
  imp_ast_free(ast_root);
  fclose(yyin);
  return 0;
}