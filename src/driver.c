#include "driver.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "interpreter.h"


typedef void *YY_BUFFER_STATE;
extern FILE *yyin;
extern IMP_ASTNode *ast_root;
extern int yyparse(void);
extern void yyrestart (FILE*);
extern YY_BUFFER_STATE yy_scan_string(const char*);
extern void yy_delete_buffer(YY_BUFFER_STATE);

int imp_driver_interpret_file (IMP_InterpreterContext *context, const char *path) {
  yyin = fopen(path, "r");
  if (!yyin) return -1;
  yyrestart(yyin);
  if (yyparse()) {
    imp_ast_destroy(ast_root);
    fclose(yyin);
    return -1;
  }
  if (imp_interpreter_interpret_ast(context, ast_root)) {
    imp_ast_destroy(ast_root);
    fclose(yyin);
    return -1;
  }
  imp_ast_destroy(ast_root);
  fclose(yyin);
  return 0;
}

int imp_driver_interpret_str (IMP_InterpreterContext *context, const char *str) {
  YY_BUFFER_STATE buf = yy_scan_string(str);
  if (yyparse()) {
    imp_ast_destroy(ast_root);
    yy_delete_buffer(buf);
    return -1;
  }
  if (imp_interpreter_interpret_ast(context, ast_root)) {
    imp_ast_destroy(ast_root);
    yy_delete_buffer(buf);
    return -1;
  }
  yy_delete_buffer(buf);
  return 0;
}

static void ast_print (IMP_ASTNode *node, int depth) {
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

int imp_driver_print_ast_file (const char *path) {
  yyin = fopen(path, "r");
  if (!yyin) return -1;
  yyrestart(yyin);
  if (yyparse()) {
    imp_ast_destroy(ast_root);
    fclose(yyin);
    return -1;
  }
  ast_print(ast_root, 0);
  imp_ast_destroy(ast_root);
  fclose(yyin);
  return 0;
}

void imp_driver_print_var_table(IMP_InterpreterContext *context) {
  IMP_InterpreterContextVarIter *iter = imp_interpreter_context_var_iter_create(context);
  const IMP_InterpreterContextVarTableEntry *var_entry;
  while ((var_entry = imp_interpreter_context_var_iter_next(iter))) {
    printf("%s = %d\n", var_entry->key, var_entry->value);
  }
  imp_interpreter_context_var_iter_destroy(iter);
}

void imp_driver_print_proc_table(IMP_InterpreterContext *context) {
  IMP_InterpreterContextProcIter *iter = imp_interpreter_context_proc_iter_create(context);
  const IMP_InterpreterContextProcTableEntry *proc_entry;
  while ((proc_entry = imp_interpreter_context_proc_iter_next(iter))) {
    const IMP_ASTNode *procdecl = proc_entry->value;
    printf("%s(", proc_entry->key);
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
  imp_interpreter_context_proc_iter_destroy(iter);
}