#include "interpreter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void env_update(Env **env, const char *name, int val) {
  for (Env *e = *env; e; e = e->next) {
    if (strcmp(e->name, name) == 0) {
      e->val = val;
      return;
    }
  }
  Env *e = malloc(sizeof(Env));
  e->name = strdup(name);
  e->val = val;
  e->next = *env;
  *env = e;
}

static int env_lookup(Env **env, const char *name) {
  for (Env *e = *env; e; e = e->next) {
    if (strcmp(e->name, name) == 0)
      return e->val;
  }
  env_update(env, name, 0);
  return env_lookup(env, name);
}

void env_print(Env *env) {
  Env *e = env;
  while (e) {
    printf("%s = %d\n", e->name, e->val);
    e = e->next;
  }
}

static int eval_aexpr(Env **env, ASTNode *node) {
  switch (node->type) {
    case NT_INT: return node->u.d_int.val;
    case NT_VAR: return env_lookup(env, node->u.d_var.name);
    case NT_AOP: {
      int aexp1 = eval_aexpr(env, node->u.d_aop.aexp1);
      int aexp2 = eval_aexpr(env, node->u.d_aop.aexp2);
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

static int eval_bexpr(Env **env, ASTNode *node) {
  switch (node->type) {
    case NT_BOP: {
      int bexp1 = eval_bexpr(env, node->u.d_bop.bexp1);
      int bexp2 = eval_bexpr(env, node->u.d_bop.bexp2);
      switch (node->u.d_bop.bop) {
        case BOP_AND: return bexp1 && bexp2;
        case BOP_OR:  return bexp1 || bexp2;
      }
    }
    case NT_NOT:
      return !eval_bexpr(env, node->u.d_not.bexp);
    case NT_ROP: {
      int aexp1 = eval_aexpr(env, node->u.d_rop.aexp1);
      int aexp2 = eval_aexpr(env, node->u.d_rop.aexp2);
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

void exec_stmt(Env **env, ASTNode *node) {
  while (node) {
    switch (node->type) {
      case NT_SKIP:
        return;
      case NT_ASSIGN: {
        char *var = node->u.d_assign.var->u.d_var.name;
        int val = eval_aexpr(env, node->u.d_assign.aexp);
        env_update(env, var, val);
        return;
      }
      case NT_SEQ:
        exec_stmt(env, node->u.d_seq.stm1);
        exec_stmt(env, node->u.d_seq.stm2);
        return;
      case NT_IF:
        if (eval_bexpr(env, node->u.d_if.bexp))
          exec_stmt(env, node->u.d_if.stm1);
        else
          exec_stmt(env, node->u.d_if.stm2);
        return;
      case NT_WHILE:
        while (eval_bexpr(env, node->u.d_while.bexp)) {
          exec_stmt(env, node->u.d_while.stm);
        }
        return;
      default:
        fprintf(stderr, "Bad stmt node %d\n", node->type);
        exit(EXIT_FAILURE);
    }
  }
}
