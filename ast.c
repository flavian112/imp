#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static ASTNode *new_node(NodeType type) {
  ASTNode *node = malloc(sizeof(ASTNode));
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

void free_ast(ASTNode *node) {
  if (!node) return;
  switch (node->type) {
    case NT_SKIP:
      break;
    case NT_ASSIGN:
      free_ast(node->u.d_assign.var);
      free_ast(node->u.d_assign.aexp);
      break;
    case NT_SEQ:
      free_ast(node->u.d_seq.stm1);
      free_ast(node->u.d_seq.stm2);
      break;
    case NT_IF:
      free_ast(node->u.d_if.bexp);
      free_ast(node->u.d_if.stm1);
      free_ast(node->u.d_if.stm2);
      break;
    case NT_WHILE:
      free_ast(node->u.d_while.bexp);
      free_ast(node->u.d_while.stm);
      break;
    case NT_INT:
      break;
    case NT_VAR:
      free(node->u.d_var.name);
      break;
    case NT_AOP:
      free_ast(node->u.d_aop.aexp1);
      free_ast(node->u.d_aop.aexp2);
      break;
    case NT_BOP:
      free_ast(node->u.d_bop.bexp1);
      free_ast(node->u.d_bop.bexp2);
      break;
    case NT_ROP:
      free_ast(node->u.d_rop.aexp1);
      free_ast(node->u.d_rop.aexp2);
      break;
    case NT_NOT:
      free_ast(node->u.d_not.bexp);
      break;
  }
  free(node);
}

typedef struct Env {
  char *name;
  int val;
  struct Env *next;
} Env;

static Env *env = NULL;

static int env_lookup(const char *name) {
  for (Env *e = env; e; e = e->next) {
    if (strcmp(e->name, name) == 0)
      return e->val;
  }
  fprintf(stderr, "Undefined variable %s\n", name);
  exit(EXIT_FAILURE);
}

static void env_update(const char *name, int val) {
  for (Env *e = env; e; e = e->next) {
    if (strcmp(e->name, name) == 0) {
      e->val = val;
      return;
    }
  }
  Env *e = malloc(sizeof(Env));
  e->name = strdup(name);
  e->val = val;
  e->next = env;
  env = e;
}

void env_print(void) {
  Env *current_env = env;
  while (current_env) {
    printf("%s = %d\n", current_env->name, current_env->val);
    current_env = current_env->next;
  }
}


static int eval_aexpr(ASTNode *node) {
  switch (node->type) {
    case NT_INT: return node->u.d_int.val;
    case NT_VAR: return env_lookup(node->u.d_var.name);
    case NT_AOP: {
      int aexp1 = eval_aexpr(node->u.d_aop.aexp1);
      int aexp2 = eval_aexpr(node->u.d_aop.aexp2);
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

static int eval_bexpr(ASTNode *node) {
  switch (node->type) {
    case NT_BOP: {
      int bexp1 = eval_bexpr(node->u.d_bop.bexp1);
      int bexp2 = eval_bexpr(node->u.d_bop.bexp2);
      switch (node->u.d_bop.bop) {
        case BOP_AND: return bexp1 && bexp2;
        case BOP_OR:  return bexp1 || bexp2;
      }
    }
    case NT_NOT:
      return !eval_bexpr(node->u.d_not.bexp);
    case NT_ROP: {
      int aexp1 = eval_aexpr(node->u.d_rop.aexp1);
      int aexp2 = eval_aexpr(node->u.d_rop.aexp2);
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

void exec_stmt(ASTNode *node) {
  while (node) {
    switch (node->type) {
      case NT_SKIP:
        return;
      case NT_ASSIGN: {
        char *var = node->u.d_assign.var->u.d_var.name;
        int val = eval_aexpr(node->u.d_assign.aexp);
        env_update(var, val);
        return;
      }
      case NT_SEQ:
        exec_stmt(node->u.d_seq.stm1);
        exec_stmt(node->u.d_seq.stm2);
        return;
      case NT_IF:
        if (eval_bexpr(node->u.d_if.bexp))
          exec_stmt(node->u.d_if.stm1);
        else
          exec_stmt(node->u.d_if.stm2);
        return;
      case NT_WHILE:
        while (eval_bexpr(node->u.d_while.bexp)) {
          exec_stmt(node->u.d_while.stm);
        }
        return;
      default:
        fprintf(stderr, "Bad stmt node %d\n", node->type);
        exit(EXIT_FAILURE);
    }
  }
}
