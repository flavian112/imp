#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ast.h"
#include "interpreter_context.h"
#include "interpreter.h"

static void test_interpreter_context(void) {
  IMP_InterpreterContext *context = imp_interpreter_context_create();
  assert(context);

  int value;

  value = imp_interpreter_context_var_get(context, "a");
  assert(value == 0);

  imp_interpreter_context_var_set(context, "a", 1);
  value = imp_interpreter_context_var_get(context, "a");
  assert(value == 1);

  imp_interpreter_context_var_set(context, "b", 2);
  value = imp_interpreter_context_var_get(context, "b");
  assert(value == 2);

  imp_interpreter_context_var_set(context, "c", 3);
  value = imp_interpreter_context_var_get(context, "c");
  assert(value == 3);

  imp_interpreter_context_var_set(context, "a", 4);
  value = imp_interpreter_context_var_get(context, "a");
  assert(value == 4);

  imp_interpreter_context_var_set(context, "b", 0);
  value = imp_interpreter_context_var_get(context, "b");
  assert(value == 0);

  IMP_InterpreterContextVarIter *var_iter = imp_interpreter_context_var_iter_create(context);
  IMP_InterpreterContextVarTableEntry *var_entry;
  while ((var_entry = imp_interpreter_context_var_iter_next(var_iter))) {
    if (strcmp(var_entry->key, "a") == 0) assert(var_entry->value == 4);
    else if (strcmp(var_entry->key, "c") == 0) assert(var_entry->value == 3);
    else assert(0);
  }
  imp_interpreter_context_var_iter_destroy(var_iter);

  IMP_ASTNode *proc_a = imp_ast_procdecl("a", NULL, NULL, imp_ast_skip());
  IMP_ASTNode *proc_b = imp_ast_procdecl("b", NULL, NULL, imp_ast_skip());
  const IMP_ASTNode *proc;

  proc = imp_interpreter_context_proc_get(context, "a");
  assert(proc == NULL);

  imp_interpreter_context_proc_set(context, "a", proc_a);
  proc = imp_interpreter_context_proc_get(context, "a");
  assert(proc != NULL);

  imp_interpreter_context_proc_set(context, "b", proc_b);
  proc = imp_interpreter_context_proc_get(context, "b");
  assert(proc != NULL);

  imp_interpreter_context_proc_set(context, "b", NULL);
  proc = imp_interpreter_context_proc_get(context, "b");
  assert(proc == NULL);

  IMP_InterpreterContextProcIter *proc_iter = imp_interpreter_context_proc_iter_create(context);
  IMP_InterpreterContextProcTableEntry *proc_entry;
  while ((proc_entry = imp_interpreter_context_proc_iter_next(proc_iter))) {
    if (strcmp(proc_entry->key, "a") == 0) assert(proc_entry->value != NULL);
    else assert(0);
  }
  imp_interpreter_context_proc_iter_destroy(proc_iter);

  imp_ast_destroy(proc_a);
  imp_ast_destroy(proc_b);
  
  imp_interpreter_context_destroy(context);
}

static void test_interpreter(void) {
  IMP_ASTNode *factorial_procdecl = imp_ast_procdecl(
    "factorial",
    imp_ast_list(imp_ast_var("n"), NULL),
    imp_ast_list(imp_ast_var("r"), NULL),
    imp_ast_if(
      imp_ast_rop(IMP_AST_ROP_LE, imp_ast_var("n"), imp_ast_int(0)),
      imp_ast_assign(imp_ast_var("r"), imp_ast_int(1)),
      imp_ast_seq(
        imp_ast_assign(imp_ast_var("m"), imp_ast_aop(IMP_AST_AOP_SUB, imp_ast_var("n"), imp_ast_int(1))),
        imp_ast_seq(
          imp_ast_proccall(
            "factorial", 
            imp_ast_list(imp_ast_var("m"), NULL),
            imp_ast_list(imp_ast_var("r"), NULL)
          ),
          imp_ast_assign(imp_ast_var("r"), imp_ast_aop(IMP_AST_AOP_MUL, imp_ast_var("r"), imp_ast_var("n")))
        )
      )
    )
  );

  IMP_ASTNode *main = imp_ast_seq(
    factorial_procdecl,
    imp_ast_seq(
      imp_ast_assign(imp_ast_var("n"), imp_ast_int(5)),
      imp_ast_proccall(
        "factorial",
        imp_ast_list(imp_ast_var("n"), NULL),
        imp_ast_list(imp_ast_var("r"), NULL)
      )
    )
  );

  IMP_InterpreterContext *context = imp_interpreter_context_create();
  int result = imp_interpreter_interpret_ast(context, main);
  assert(result == 0);
  int factorial_result = imp_interpreter_context_var_get(context, "r");
  assert(factorial_result == 120);
  const IMP_ASTNode *proc = imp_interpreter_context_proc_get(context, "factorial");
  assert(proc != NULL);

  imp_ast_destroy(main);
  imp_interpreter_context_destroy(context);
}

int main(void) {
  printf("Starting tests...\n");
  test_interpreter_context();
  test_interpreter();
  printf("All tests passed\n");
}