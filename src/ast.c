#include "ast.h"
#include <stdio.h>
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
  }
  free(node);
}