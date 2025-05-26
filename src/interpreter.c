#include "interpreter.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


static int eval_aexpr(IMP_InterpreterContext *context, const IMP_ASTNode *node) {
  switch (node->type) {
    case IMP_AST_NT_INT: return node->data.integer.val;
    case IMP_AST_NT_VAR: return imp_interpreter_context_var_get(context, node->data.variable.name);
    case IMP_AST_NT_AOP: {
      int l_val = eval_aexpr(context, node->data.arith_op.l_aexpr);
      int r_val = eval_aexpr(context, node->data.arith_op.r_aexpr);
      switch (node->data.arith_op.aopr) {
        case IMP_AST_AOP_ADD: return l_val + r_val;
        case IMP_AST_AOP_SUB: return l_val - r_val;
        case IMP_AST_AOP_MUL: return l_val * r_val;
        default: assert(0);
      }
    }
    default: assert(0);
  }
}

static int eval_bexpr(IMP_InterpreterContext *context, const IMP_ASTNode *node) {
  switch (node->type) {
    case IMP_AST_NT_BOP: {
      int l_val = eval_bexpr(context, node->data.bool_op.l_bexpr);
      int r_val = eval_bexpr(context, node->data.bool_op.r_bexpr);
      switch (node->data.bool_op.bopr) {
        case IMP_AST_BOP_AND: return l_val && r_val;
        case IMP_AST_BOP_OR:  return l_val || r_val;
        default: assert(0);
      }
    }
    case IMP_AST_NT_NOT: return !eval_bexpr(context, node->data.bool_not.bexpr);
    case IMP_AST_NT_ROP: {
      int l_val = eval_aexpr(context, node->data.rel_op.l_aexpr);
      int r_val = eval_aexpr(context, node->data.rel_op.r_aexpr);
      switch (node->data.rel_op.ropr) {
        case IMP_AST_ROP_EQ: return l_val == r_val;
        case IMP_AST_ROP_NE: return l_val != r_val;
        case IMP_AST_ROP_LT: return l_val < r_val;
        case IMP_AST_ROP_LE: return l_val <= r_val;
        case IMP_AST_ROP_GT: return l_val > r_val;
        case IMP_AST_ROP_GE: return l_val >= r_val;
        default: assert(0);
      }
    }
    default: assert(0);
  }
}

static int interpret_proccall(IMP_InterpreterContext *context, const IMP_ASTNode *node) {
  const char *name = node->data.proc_call.name;
  const IMP_ASTNode *procdecl = imp_interpreter_context_proc_get(context, name);
  if (!procdecl) {
    fprintf(stderr, "Error: procedure %s not defined\n", name);
    return -1;
  }
  IMP_InterpreterContext *proc_context = imp_interpreter_context_create();
  IMP_InterpreterContextProcIter *proc_iter = imp_interpreter_context_proc_iter_create(context);
  const IMP_InterpreterContextProcTableEntry *proc_entry;
  while ((proc_entry = imp_interpreter_context_proc_iter_next(proc_iter))) {
    imp_interpreter_context_proc_set(proc_context, proc_entry->key, proc_entry->value);
  }
  imp_interpreter_context_proc_iter_destroy(proc_iter);
  IMP_ASTNodeList *caller_val_args = node->data.proc_call.val_args;
  IMP_ASTNodeList *callee_val_args = procdecl->data.proc_decl.val_args;
  while (caller_val_args && callee_val_args) {
    int val = eval_aexpr(context, caller_val_args->node);
    const char *callee_arg_name = callee_val_args->node->data.variable.name;
    imp_interpreter_context_var_set(proc_context, callee_arg_name, val);
    caller_val_args = caller_val_args->next;
    callee_val_args = callee_val_args->next;
  }
  if (caller_val_args || callee_val_args) {
    fprintf(stderr, "Error: procedure %s called with wrong number of value arguments\n", name);
    imp_interpreter_context_destroy(proc_context);
    return -1;
  }
  if (imp_interpreter_interpret_ast(proc_context, procdecl->data.proc_decl.body_stmt)) {
    imp_interpreter_context_destroy(proc_context);
    return -1;
  }
  IMP_ASTNodeList *caller_var_args = node->data.proc_call.var_args;
  IMP_ASTNodeList *callee_var_args = procdecl->data.proc_decl.var_args;
  while (caller_var_args && callee_var_args) {
    const char *caller_varg_name = caller_var_args->node->data.variable.name;
    const char *callee_varg_name = callee_var_args->node->data.variable.name;
    imp_interpreter_context_var_set(context, caller_varg_name, imp_interpreter_context_var_get(proc_context, callee_varg_name));
    caller_var_args = caller_var_args->next;
    callee_var_args = callee_var_args->next;
  }
  if (caller_var_args || callee_var_args) {
    fprintf(stderr, "Error: procedure %s called with wrong number of variable arguments\n", name);
    imp_interpreter_context_destroy(proc_context);
    return -1;
  }
  imp_interpreter_context_destroy(proc_context);
  return 0;
}

int imp_interpreter_interpret_ast(IMP_InterpreterContext *context, const IMP_ASTNode *node) {
  switch (node->type) {
    case IMP_AST_NT_SKIP: return 0;
    case IMP_AST_NT_ASSIGN: {
      const char *name = node->data.assign.var->data.variable.name;
      int val = eval_aexpr(context, node->data.assign.aexpr);
      imp_interpreter_context_var_set(context, name, val);
      return 0;
    }
    case IMP_AST_NT_SEQ:
      if (imp_interpreter_interpret_ast(context, node->data.seq.fst_stmt)) return -1;
      if (imp_interpreter_interpret_ast(context, node->data.seq.snd_stmt)) return -1;
      return 0;
    case IMP_AST_NT_IF:
      if (eval_bexpr(context, node->data.if_stmt.cond_bexpr)) return imp_interpreter_interpret_ast(context, node->data.if_stmt.then_stmt);
      else return imp_interpreter_interpret_ast(context, node->data.if_stmt.else_stmt);
    case IMP_AST_NT_WHILE:
      while (eval_bexpr(context, node->data.while_stmt.cond_bexpr)) {
        if (imp_interpreter_interpret_ast(context, node->data.while_stmt.body_stmt)) return -1;
      }
      return 0;
    case IMP_AST_NT_LET: {
      const char *name = node->data.let_stmt.var->data.variable.name;
      int old_val = imp_interpreter_context_var_get(context, name);
      int new_val = eval_aexpr(context, node->data.let_stmt.aexpr);
      imp_interpreter_context_var_set(context, name, new_val);
      int ret = imp_interpreter_interpret_ast(context, node->data.let_stmt.body_stmt);
      imp_interpreter_context_var_set(context, name, old_val);
      return ret;
    }
    case IMP_AST_NT_PROCDECL: {
      const char *name = node->data.proc_decl.name;
      if (imp_interpreter_context_proc_get(context, name)) {
        fprintf(stderr, "Error: procedure %s already defined\n", name);
        return -1;
      }
      imp_interpreter_context_proc_set(context, name, node);
      return 0;
    }
    case IMP_AST_NT_PROCCALL: {
      return interpret_proccall(context, node);
    }
    default: assert(0);
  }
}