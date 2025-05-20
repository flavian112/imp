#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static ASTNode *new_node(NodeType type) {
  ASTNode *n = malloc(sizeof(ASTNode));
  if (!n) {
    fprintf(stderr, "Out of memory\n");
    exit(EXIT_FAILURE);
  }
  n->type = type;
  return n;
}

ASTNode *ast_skip(void) {
  return new_node(NT_SKIP);
}

ASTNode *ast_assign(ASTNode *var, ASTNode *aexp) {
  ASTNode *n = new_node(NT_ASSIGN);
  n->u.assign.var = var;
  n->u.assign.aexp = aexp;
  return n;
}

ASTNode *ast_seq(ASTNode *s1, ASTNode *s2) {
  ASTNode *n = new_node(NT_SEQ);
  n->u.seq.s1 = s1;
  n->u.seq.s2 = s2;
  return n;
}

ASTNode *ast_if(ASTNode *bexp, ASTNode *s1, ASTNode *s2) {
  ASTNode *n = new_node(NT_IF);
  n->u.cond.bexp = bexp;
  n->u.cond.s1 = s1;
  n->u.cond.s2 = s2;
  return n;
}

ASTNode *ast_while(ASTNode *bexp, ASTNode *s) {
  ASTNode *n = new_node(NT_WHILE);
  n->u.cond.bexp = bexp;
  n->u.cond.s1 = s;
  return n;
}

ASTNode *ast_int(int val) {
  ASTNode *n = new_node(NT_INT);
  n->u.val = val;
  return n;
}

ASTNode *ast_var(char *var) {
  ASTNode *n = new_node(NT_VAR);
  n->u.var = strdup(var);
  return n;
}

ASTNode *ast_aop(AOp op, ASTNode *aexp1, ASTNode *aexp2) {
  ASTNode *n = new_node(NT_AOP);
  n->u.bin.exp1 = aexp1;
  n->u.bin.exp2 = aexp2;
  n->u.bin.op.aop = op;
  return n;
}

ASTNode *ast_bop(BOp op, ASTNode *bexp1, ASTNode *bexp2) {
  ASTNode *n = new_node(NT_BOP);
  n->u.bin.exp1 = bexp1;
  n->u.bin.exp2 = bexp2;
  n->u.bin.op.bop = op;
  return n;
}

ASTNode *ast_not(ASTNode *bexp) {
  ASTNode *n = new_node(NT_NOT);
  n->u.bexp = bexp;
  return n;
}

ASTNode *ast_rop(ROp op, ASTNode *aexp1, ASTNode *aexp2) {
  ASTNode *n = new_node(NT_ROP);
  n->u.bin.exp1 = aexp1;
  n->u.bin.exp2 = aexp2;
  n->u.bin.op.rop = op;
  return n;
}

void free_ast(ASTNode *n) {
  if (!n) return;
  switch (n->type) {
    case NT_SKIP:
      break;
    case NT_ASSIGN:
      free_ast(n->u.assign.var);
      free_ast(n->u.assign.aexp);
      break;
    case NT_SEQ:
      free_ast(n->u.seq.s1);
      free_ast(n->u.seq.s2);
      break;
    case NT_IF:
      free_ast(n->u.cond.bexp);
      free_ast(n->u.cond.s1);
      free_ast(n->u.cond.s2);
      break;
    case NT_WHILE:
      free_ast(n->u.cond.bexp);
      free_ast(n->u.cond.s1);
      break;
    case NT_INT:
      break;
    case NT_VAR:
      free(n->u.var);
      break;
    case NT_AOP:
    case NT_BOP:
    case NT_ROP:
      free_ast(n->u.bin.exp1);
      free_ast(n->u.bin.exp2);
      break;
    case NT_NOT:
      free_ast(n->u.bexp);
      break;
  }
  free(n);
}

typedef struct Env {
  char *var;
  int val;
  struct Env *next;
} Env;

static Env *env = NULL;

static int env_lookup(const char *var) {
  for (Env *e = env; e; e = e->next) {
    if (strcmp(e->var, var) == 0)
      return e->val;
  }
  fprintf(stderr, "Undefined variable %s\n", var);
  exit(EXIT_FAILURE);
}

static void env_update(const char *var, int val) {
  for (Env *e = env; e; e = e->next) {
    if (strcmp(e->var, var) == 0) {
      e->val = val;
      return;
    }
  }
  Env *e = malloc(sizeof(Env));
  e->var = strdup(var);
  e->val = val;
  e->next = env;
  env = e;
}

void env_print(void) {
  Env *current_env = env;
  while (current_env) {
    printf("%s = %d\n", current_env->var, current_env->val);
    current_env = current_env->next;
  }
}


static int eval_aexpr(ASTNode *n) {
  switch (n->type) {
    case NT_INT: return n->u.val;
    case NT_VAR: return env_lookup(n->u.var);
    case NT_AOP: {
      int aexp1 = eval_aexpr(n->u.bin.exp1);
      int aexp2 = eval_aexpr(n->u.bin.exp2);
      switch (n->u.bin.op.aop) {
        case AOP_ADD: return aexp1 + aexp2;
        case AOP_SUB: return aexp1 - aexp2;
        case AOP_MUL: return aexp1 * aexp2;
      }
    }
    default:
      fprintf(stderr, "Bad aexpr node %d\n", n->type);
      exit(EXIT_FAILURE);
  }
}

static int eval_bexpr(ASTNode *n) {
  switch (n->type) {
    case NT_BOP: {
      int bexp1 = eval_bexpr(n->u.bin.exp1);
      int bexp2 = eval_bexpr(n->u.bin.exp2);
      switch (n->u.bin.op.bop) {
        case BOP_AND: return bexp1 && bexp2;
        case BOP_OR:  return bexp1 || bexp2;
      }
    }
    case NT_NOT:
      return !eval_bexpr(n->u.bexp);
    case NT_ROP: {
      int aexp1 = eval_aexpr(n->u.bin.exp1);
      int aexp2 = eval_aexpr(n->u.bin.exp2);
      switch (n->u.bin.op.rop) {
        case ROP_EQ: return aexp1 == aexp2;
        case ROP_NE: return aexp1 != aexp2;
        case ROP_LT: return aexp1 < aexp2;
        case ROP_LE: return aexp1 <= aexp2;
        case ROP_GT: return aexp1 > aexp2;
        case ROP_GE: return aexp1 >= aexp2;
      }
    }
    default:
      fprintf(stderr, "Bad bexpr node %d\n", n->type);
      exit(EXIT_FAILURE);
  }
}

void exec_stmt(ASTNode *n) {
  while (n) {
    switch (n->type) {
      case NT_SKIP:
        return;
      case NT_ASSIGN: {
        char *var = n->u.assign.var->u.var;
        int val = eval_aexpr(n->u.assign.aexp);
        env_update(var, val);
        return;
      }
      case NT_SEQ:
        exec_stmt(n->u.seq.s1);
        n = n->u.seq.s2;
        continue;
      case NT_IF:
        if (eval_bexpr(n->u.cond.bexp))
          exec_stmt(n->u.cond.s1);
        else
          exec_stmt(n->u.cond.s2);
        return;
      case NT_WHILE:
        while (eval_bexpr(n->u.cond.bexp)) {
          exec_stmt(n->u.cond.s1);
        }
        return;
      default:
        fprintf(stderr, "Bad stmt node %d\n", n->type);
        exit(EXIT_FAILURE);
    }
  }
}
