#include "interpreter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void context_set(hashmap_t context, const char *name, int val) {
  hashmap_insert(context, name, val);
}

static int context_get(hashmap_t context, const char *name) {
  int *val = hashmap_get(context, name);
  if (val) return *val;
  return 0;
}

static void print_var(const char *name, int val) {
  printf("%s = %d\n", name, val);
}

void context_print(hashmap_t context) {
  hashmap_iterate(context, print_var);
}

static int eval_aexpr(hashmap_t context, ASTNode *node) {
  switch (node->type) {
    case NT_INT: return node->u.d_int.val;
    case NT_VAR: return context_get(context, node->u.d_var.name);
    case NT_AOP: {
      int aexp1 = eval_aexpr(context, node->u.d_aop.aexp1);
      int aexp2 = eval_aexpr(context, node->u.d_aop.aexp2);
      switch (node->u.d_aop.aop) {
        case AOP_ADD: return aexp1 + aexp2;
        case AOP_SUB: return aexp1 - aexp2;
        case AOP_MUL: return aexp1 * aexp2;
      }
    }
    default:
      fprintf(stderr, "Bad aexpr node %d\n", node->type);
      exit(EXIT_FAILURE);
  }
}

static int eval_bexpr(hashmap_t context, ASTNode *node) {
  switch (node->type) {
    case NT_BOP: {
      int bexp1 = eval_bexpr(context, node->u.d_bop.bexp1);
      int bexp2 = eval_bexpr(context, node->u.d_bop.bexp2);
      switch (node->u.d_bop.bop) {
        case BOP_AND: return bexp1 && bexp2;
        case BOP_OR:  return bexp1 || bexp2;
      }
    }
    case NT_NOT:
      return !eval_bexpr(context, node->u.d_not.bexp);
    case NT_ROP: {
      int aexp1 = eval_aexpr(context, node->u.d_rop.aexp1);
      int aexp2 = eval_aexpr(context, node->u.d_rop.aexp2);
      switch (node->u.d_rop.rop) {
        case ROP_EQ: return aexp1 == aexp2;
        case ROP_NE: return aexp1 != aexp2;
        case ROP_LT: return aexp1 < aexp2;
        case ROP_LE: return aexp1 <= aexp2;
        case ROP_GT: return aexp1 > aexp2;
        case ROP_GE: return aexp1 >= aexp2;
      }
    }
    default:
      fprintf(stderr, "Bad bexpr node %d\n", node->type);
      exit(EXIT_FAILURE);
  }
}

void exec_stmt(hashmap_t context, ASTNode *node) {
  while (node) {
    switch (node->type) {
      case NT_SKIP:
        return;
      case NT_ASSIGN: {
        char *var = node->u.d_assign.var->u.d_var.name;
        int val = eval_aexpr(context, node->u.d_assign.aexp);
        context_set(context, var, val);
        return;
      }
      case NT_SEQ:
        exec_stmt(context, node->u.d_seq.stm1);
        exec_stmt(context, node->u.d_seq.stm2);
        return;
      case NT_IF:
        if (eval_bexpr(context, node->u.d_if.bexp))
          exec_stmt(context, node->u.d_if.stm1);
        else
          exec_stmt(context, node->u.d_if.stm2);
        return;
      case NT_WHILE:
        while (eval_bexpr(context, node->u.d_while.bexp)) {
          exec_stmt(context, node->u.d_while.stm);
        }
        return;
      default:
        fprintf(stderr, "Bad stmt node %d\n", node->type);
        exit(EXIT_FAILURE);
    }
  }
}
