#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>


static ASTNode *new_node(NodeType type) {
  ASTNode *node = malloc(sizeof(ASTNode));
  assert(node);
  node->type = type;
  return node;
}

ASTNode *ast_skip(void) {
  return new_node(NT_SKIP);
}

ASTNode *ast_assign(ASTNode *var, ASTNode *aexp) {
  ASTNode *node = new_node(NT_ASSIGN);
  node->u.d_assign.var = var;
  node->u.d_assign.aexp = aexp;
  return node;
}

ASTNode *ast_seq(ASTNode *stm1, ASTNode *stm2) {
  ASTNode *node = new_node(NT_SEQ);
  node->u.d_seq.stm1 = stm1;
  node->u.d_seq.stm2 = stm2;
  return node;
}

ASTNode *ast_if(ASTNode *bexp, ASTNode *stm1, ASTNode *stm2) {
  ASTNode *node = new_node(NT_IF);
  node->u.d_if.bexp = bexp;
  node->u.d_if.stm1 = stm1;
  node->u.d_if.stm2 = stm2;
  return node;
}

ASTNode *ast_while(ASTNode *bexp, ASTNode *stm) {
  ASTNode *node = new_node(NT_WHILE);
  node->u.d_while.bexp = bexp;
  node->u.d_while.stm = stm;
  return node;
}

ASTNode *ast_int(int val) {
  ASTNode *node = new_node(NT_INT);
  node->u.d_int.val = val;
  return node;
}

ASTNode *ast_var(char *name) {
  ASTNode *node = new_node(NT_VAR);
  node->u.d_var.name = strdup(name);
  assert(node->u.d_var.name);
  return node;
}

ASTNode *ast_aop(AOp aop, ASTNode *aexp1, ASTNode *aexp2) {
  ASTNode *node = new_node(NT_AOP);
  node->u.d_aop.aexp1 = aexp1;
  node->u.d_aop.aexp2 = aexp2;
  node->u.d_aop.aop = aop;
  return node;
}

ASTNode *ast_bop(BOp bop, ASTNode *bexp1, ASTNode *bexp2) {
  ASTNode *node = new_node(NT_BOP);
  node->u.d_bop.bexp1 = bexp1;
  node->u.d_bop.bexp2 = bexp2;
  node->u.d_bop.bop = bop;
  return node;
}

ASTNode *ast_not(ASTNode *bexp) {
  ASTNode *node = new_node(NT_NOT);
  node->u.d_not.bexp = bexp;
  return node;
}

ASTNode *ast_rop(ROp rop, ASTNode *aexp1, ASTNode *aexp2) {
  ASTNode *node = new_node(NT_ROP);
  node->u.d_rop.aexp1 = aexp1;
  node->u.d_rop.aexp2 = aexp2;
  node->u.d_rop.rop = rop;
  return node;
}

ASTNode *ast_let(ASTNode *var, ASTNode *aexp, ASTNode *stm) {
  ASTNode *node = new_node(NT_LET);
  node->u.d_let.var = var;
  node->u.d_let.aexp = aexp;
  node->u.d_let.stm = stm;
  return node;
}

ASTNode *ast_procdecl(const char *name, ASTNodeList *args, ASTNodeList *vargs, ASTNode *stm) {
  ASTNode *node = new_node(NT_PROCDECL);
  node->u.d_procdecl.name = strdup(name);
  assert(node->u.d_procdecl.name);
  node->u.d_procdecl.args = args;
  node->u.d_procdecl.vargs = vargs;
  node->u.d_procdecl.stm = stm;
  return node;
}

ASTNode *ast_proccall(const char *name, ASTNodeList *args, ASTNodeList *vargs) {
  ASTNode *node = new_node(NT_PROCCALL);
  node->u.d_proccall.name = strdup(name);
  assert(node->u.d_proccall.name);
  node->u.d_proccall.args = args;
  node->u.d_proccall.vargs = vargs;
  return node;
}

ASTNodeList *ast_node_list(ASTNode *node, ASTNodeList *list) {
  ASTNodeList *new_list = malloc(sizeof(ASTNodeList));
  assert(new_list);
  new_list->node = node;
  new_list->next = list;
  return new_list;
}

static void ast_free_node_list(ASTNodeList *list) {
  if (!list) return;
  ast_free(list->node);
  ast_free_node_list(list->next);
  free(list);
}

void ast_free(ASTNode *node) {
  if (!node) return;
  switch (node->type) {
    case NT_SKIP:
      break;
    case NT_ASSIGN:
      ast_free(node->u.d_assign.var);
      ast_free(node->u.d_assign.aexp);
      break;
    case NT_SEQ:
      ast_free(node->u.d_seq.stm1);
      ast_free(node->u.d_seq.stm2);
      break;
    case NT_IF:
      ast_free(node->u.d_if.bexp);
      ast_free(node->u.d_if.stm1);
      ast_free(node->u.d_if.stm2);
      break;
    case NT_WHILE:
      ast_free(node->u.d_while.bexp);
      ast_free(node->u.d_while.stm);
      break;
    case NT_INT:
      break;
    case NT_VAR:
      free(node->u.d_var.name);
      break;
    case NT_AOP:
      ast_free(node->u.d_aop.aexp1);
      ast_free(node->u.d_aop.aexp2);
      break;
    case NT_BOP:
      ast_free(node->u.d_bop.bexp1);
      ast_free(node->u.d_bop.bexp2);
      break;
    case NT_ROP:
      ast_free(node->u.d_rop.aexp1);
      ast_free(node->u.d_rop.aexp2);
      break;
    case NT_NOT:
      ast_free(node->u.d_not.bexp);
      break;
    case NT_LET:
      ast_free(node->u.d_let.var);
      ast_free(node->u.d_let.aexp);
      ast_free(node->u.d_let.stm);
      break;
    case NT_PROCDECL:
      free(node->u.d_procdecl.name);
      ast_free_node_list(node->u.d_procdecl.args);
      ast_free_node_list(node->u.d_procdecl.vargs);
      ast_free(node->u.d_procdecl.stm);
      break;
    case NT_PROCCALL:
      free(node->u.d_proccall.name);
      ast_free_node_list(node->u.d_proccall.args);
      ast_free_node_list(node->u.d_proccall.vargs);
      break;
    default: assert(0);
  }
  free(node);
}

static ASTNodeList *ast_node_list_clone(ASTNodeList *list) {
  if (!list) return NULL;
  return ast_node_list(ast_clone(list->node), ast_node_list_clone(list->next));
}

ASTNode *ast_clone(ASTNode *node) {
  if (!node) return NULL;
  switch (node->type) {
    case NT_SKIP: return ast_skip();
    case NT_ASSIGN: return ast_assign(ast_clone(node->u.d_assign.var), ast_clone(node->u.d_assign.aexp));
    case NT_SEQ: return ast_seq(ast_clone(node->u.d_seq.stm1), ast_clone(node->u.d_seq.stm2));
    case NT_IF: return ast_if(
      ast_clone(node->u.d_if.bexp),
      ast_clone(node->u.d_if.stm1),
      ast_clone(node->u.d_if.stm2));
    case NT_WHILE: return ast_while(
      ast_clone(node->u.d_while.bexp),
      ast_clone(node->u.d_while.stm));
    case NT_INT: return ast_int(node->u.d_int.val);
    case NT_VAR: return ast_var(node->u.d_var.name);
    case NT_AOP: return ast_aop(
      node->u.d_aop.aop,
      ast_clone(node->u.d_aop.aexp1),
      ast_clone(node->u.d_aop.aexp2));
    case NT_BOP: return ast_bop(
      node->u.d_bop.bop,
      ast_clone(node->u.d_bop.bexp1),
      ast_clone(node->u.d_bop.bexp2));
    case NT_NOT: return ast_not(ast_clone(node->u.d_not.bexp));
    case NT_ROP: return ast_rop(
      node->u.d_rop.rop,
      ast_clone(node->u.d_rop.aexp1),
      ast_clone(node->u.d_rop.aexp2));
    case NT_LET: return ast_let(
      ast_clone(node->u.d_let.var),
      ast_clone(node->u.d_let.aexp),
      ast_clone(node->u.d_let.stm));
    case NT_PROCDECL: return ast_procdecl(
      node->u.d_procdecl.name,
      ast_node_list_clone(node->u.d_procdecl.args),
      ast_node_list_clone(node->u.d_procdecl.vargs),
      ast_clone(node->u.d_procdecl.stm));
    case NT_PROCCALL: return ast_proccall(
      node->u.d_proccall.name,
      ast_node_list_clone(node->u.d_proccall.args),
      ast_node_list_clone(node->u.d_proccall.vargs));
    default: assert(0);
  }
}