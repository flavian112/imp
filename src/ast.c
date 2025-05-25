#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>


static IMP_ASTNode *ast_create(IMP_ASTNodeType type) {
  IMP_ASTNode *node = malloc(sizeof(IMP_ASTNode));
  assert(node && "Memory allocation failed");
  node->type = type;
  return node;
}

static IMP_ASTNodeList *ast_list_clone(IMP_ASTNodeList *list) {
  if (!list) return NULL;
  return imp_ast_list(imp_ast_clone(list->node), ast_list_clone(list->next));
}

IMP_ASTNode *imp_ast_skip(void) {
  return ast_create(IMP_AST_NT_SKIP);
}

IMP_ASTNode *imp_ast_assign(IMP_ASTNode *var, IMP_ASTNode *aexpr) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_ASSIGN);
  node->data.assign.var = var;
  node->data.assign.aexpr = aexpr;
  return node;
}

IMP_ASTNode *imp_ast_seq(IMP_ASTNode *fst_stmt, IMP_ASTNode *snd_stmt) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_SEQ);
  node->data.seq.fst_stmt = fst_stmt;
  node->data.seq.snd_stmt = snd_stmt;
  return node;
}

IMP_ASTNode *imp_ast_if(IMP_ASTNode *cond_bexpr, IMP_ASTNode *then_stmt, IMP_ASTNode *else_stmt) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_IF);
  node->data.if_stmt.cond_bexpr = cond_bexpr;
  node->data.if_stmt.then_stmt = then_stmt;
  node->data.if_stmt.else_stmt = else_stmt;
  return node;
}

IMP_ASTNode *imp_ast_while(IMP_ASTNode *cond_bexpr, IMP_ASTNode *body_stmt) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_WHILE);
  node->data.while_stmt.cond_bexpr = cond_bexpr;
  node->data.while_stmt.body_stmt = body_stmt;
  return node;
}

IMP_ASTNode *imp_ast_int(int val) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_INT);
  node->data.integer.val = val;
  return node;
}

IMP_ASTNode *imp_ast_var(const char *name) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_VAR);
  node->data.variable.name = strdup(name);
  assert(node->data.variable.name && "Memory allocation failed");
  return node;
}

IMP_ASTNode *imp_ast_aop(IMP_ASTArithmeticOperator aopr, IMP_ASTNode *l_aexpr, IMP_ASTNode *r_aexpr) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_AOP);
  node->data.arith_op.aopr = aopr;
  node->data.arith_op.l_aexpr = l_aexpr;
  node->data.arith_op.r_aexpr = r_aexpr;
  return node;
}

IMP_ASTNode *imp_ast_bop(IMP_ASTBooleanOperator bopr, IMP_ASTNode *l_bexpr, IMP_ASTNode *r_bexpr) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_BOP);
  node->data.bool_op.bopr = bopr;
  node->data.bool_op.l_bexpr = l_bexpr;
  node->data.bool_op.r_bexpr = r_bexpr;
  return node;
}

IMP_ASTNode *imp_ast_not(IMP_ASTNode *bexpr) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_NOT);
  node->data.bool_not.bexpr = bexpr;
  return node;
}

IMP_ASTNode *imp_ast_rop(IMP_ASTRelationalOperator ropr, IMP_ASTNode *l_aexpr, IMP_ASTNode *r_aexpr) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_ROP);
  node->data.rel_op.ropr = ropr;
  node->data.rel_op.l_aexpr = l_aexpr;
  node->data.rel_op.r_aexpr = r_aexpr;
  return node;
}

IMP_ASTNode *imp_ast_let(IMP_ASTNode *var, IMP_ASTNode *aexpr, IMP_ASTNode *body_stmt) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_LET);
  node->data.let_stmt.var = var;
  node->data.let_stmt.aexpr = aexpr;
  node->data.let_stmt.body_stmt = body_stmt;
  return node;
}

IMP_ASTNode *imp_ast_procdecl(const char *name, IMP_ASTNodeList *val_args, IMP_ASTNodeList *var_args, IMP_ASTNode *body_stmt) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_PROCDECL);
  node->data.proc_decl.name = strdup(name);
  assert(node->data.proc_decl.name && "Memory allocation failed");
  node->data.proc_decl.val_args = val_args;
  node->data.proc_decl.var_args = var_args;
  node->data.proc_decl.body_stmt = body_stmt;
  return node;
}

IMP_ASTNode *imp_ast_proccall(const char *name, IMP_ASTNodeList *val_args, IMP_ASTNodeList *var_args) {
  IMP_ASTNode *node = ast_create(IMP_AST_NT_PROCCALL);
  node->data.proc_call.name = strdup(name);
  assert(node->data.proc_call.name && "Memory allocation failed");
  node->data.proc_call.val_args = val_args;
  node->data.proc_call.var_args = var_args;
  return node;
}

IMP_ASTNode *imp_ast_clone(const IMP_ASTNode *node) {
  if (!node) return NULL;
  switch (node->type) {
    case IMP_AST_NT_SKIP: return imp_ast_skip();
    case IMP_AST_NT_ASSIGN: return imp_ast_assign(
      imp_ast_clone(node->data.assign.var), 
      imp_ast_clone(node->data.assign.aexpr));
    case IMP_AST_NT_SEQ: return imp_ast_seq(
      imp_ast_clone(node->data.seq.fst_stmt), 
      imp_ast_clone(node->data.seq.snd_stmt));
    case IMP_AST_NT_IF: return imp_ast_if(
      imp_ast_clone(node->data.if_stmt.cond_bexpr),
      imp_ast_clone(node->data.if_stmt.then_stmt),
      imp_ast_clone(node->data.if_stmt.else_stmt));
    case IMP_AST_NT_WHILE: return imp_ast_while(
      imp_ast_clone(node->data.while_stmt.cond_bexpr),
      imp_ast_clone(node->data.while_stmt.body_stmt));
    case IMP_AST_NT_INT: return imp_ast_int(node->data.integer.val);
    case IMP_AST_NT_VAR: return imp_ast_var(node->data.variable.name);
    case IMP_AST_NT_AOP: return imp_ast_aop(
      node->data.arith_op.aopr,
      imp_ast_clone(node->data.arith_op.l_aexpr),
      imp_ast_clone(node->data.arith_op.r_aexpr));
    case IMP_AST_NT_BOP: return imp_ast_bop(
      node->data.bool_op.bopr,
      imp_ast_clone(node->data.bool_op.l_bexpr),
      imp_ast_clone(node->data.bool_op.r_bexpr));
    case IMP_AST_NT_NOT: return imp_ast_not(
      imp_ast_clone(node->data.bool_not.bexpr));
    case IMP_AST_NT_ROP: return imp_ast_rop(
      node->data.rel_op.ropr,
      imp_ast_clone(node->data.rel_op.l_aexpr),
      imp_ast_clone(node->data.rel_op.r_aexpr));
    case IMP_AST_NT_LET: return imp_ast_let(
      imp_ast_clone(node->data.let_stmt.var),
      imp_ast_clone(node->data.let_stmt.aexpr),
      imp_ast_clone(node->data.let_stmt.body_stmt));
    case IMP_AST_NT_PROCDECL: return imp_ast_procdecl(
      node->data.proc_decl.name,
      ast_list_clone(node->data.proc_decl.val_args),
      ast_list_clone(node->data.proc_decl.var_args),
      imp_ast_clone(node->data.proc_decl.body_stmt));
    case IMP_AST_NT_PROCCALL: return imp_ast_proccall(
      node->data.proc_call.name,
      ast_list_clone(node->data.proc_call.val_args),
      ast_list_clone(node->data.proc_call.var_args));
    default: assert(0 && "Unknown AST node type");
  }
}

void imp_ast_destroy(IMP_ASTNode *node) {
  if (!node) return;
  switch (node->type) {
    case IMP_AST_NT_SKIP:
      break;
    case IMP_AST_NT_ASSIGN:
      imp_ast_destroy(node->data.assign.var);
      imp_ast_destroy(node->data.assign.aexpr);
      break;
    case IMP_AST_NT_SEQ:
      imp_ast_destroy(node->data.seq.fst_stmt);
      imp_ast_destroy(node->data.seq.snd_stmt);
      break;
    case IMP_AST_NT_IF:
      imp_ast_destroy(node->data.if_stmt.cond_bexpr);
      imp_ast_destroy(node->data.if_stmt.then_stmt);
      imp_ast_destroy(node->data.if_stmt.else_stmt);
      break;
    case IMP_AST_NT_WHILE:
      imp_ast_destroy(node->data.while_stmt.cond_bexpr);
      imp_ast_destroy(node->data.while_stmt.body_stmt);
      break;
    case IMP_AST_NT_INT:
      break;
    case IMP_AST_NT_VAR:
      free(node->data.variable.name);
      break;
    case IMP_AST_NT_AOP:
      imp_ast_destroy(node->data.arith_op.l_aexpr);
      imp_ast_destroy(node->data.arith_op.r_aexpr);
      break;
    case IMP_AST_NT_BOP:
      imp_ast_destroy(node->data.bool_op.l_bexpr);
      imp_ast_destroy(node->data.bool_op.r_bexpr);
      break;
    case IMP_AST_NT_ROP:
      imp_ast_destroy(node->data.rel_op.l_aexpr);
      imp_ast_destroy(node->data.rel_op.r_aexpr);
      break;
    case IMP_AST_NT_NOT:
      imp_ast_destroy(node->data.bool_not.bexpr);
      break;
    case IMP_AST_NT_LET:
      imp_ast_destroy(node->data.let_stmt.var);
      imp_ast_destroy(node->data.let_stmt.aexpr);
      imp_ast_destroy(node->data.let_stmt.body_stmt);
      break;
    case IMP_AST_NT_PROCDECL:
      free(node->data.proc_decl.name);
      imp_ast_list_destroy(node->data.proc_decl.val_args);
      imp_ast_list_destroy(node->data.proc_decl.var_args);
      imp_ast_destroy(node->data.proc_decl.body_stmt);
      break;
    case IMP_AST_NT_PROCCALL:
      free(node->data.proc_call.name);
      imp_ast_list_destroy(node->data.proc_call.val_args);
      imp_ast_list_destroy(node->data.proc_call.var_args);
      break;
    default: assert(0 && "Unknown AST node type");
  }
  free(node);
}

IMP_ASTNodeList *imp_ast_list(IMP_ASTNode *node, IMP_ASTNodeList *list) {
  IMP_ASTNodeList *new_list = malloc(sizeof(IMP_ASTNodeList));
  assert(new_list && "Memory allocation failed");
  new_list->node = node;
  new_list->next = list;
  return new_list;
}

void imp_ast_list_destroy(IMP_ASTNodeList *list) {
  if (!list) return;
  imp_ast_destroy(list->node);
  imp_ast_list_destroy(list->next);
  free(list);
}