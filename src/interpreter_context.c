#include "interpreter_context.h"

#define STB_DS_IMPLEMENTATION
#include "3rdparty/stb_ds/stb_ds.h"


struct IMP_InterpreterContext {
  IMP_InterpreterContextVarTableEntry *var_table;
  IMP_InterpreterContextProcTableEntry *proc_table;
};

struct IMP_InterpreterContextVarIter {
  IMP_InterpreterContextVarTableEntry *var_table;
  ptrdiff_t index;
  ptrdiff_t len;
};

struct IMP_InterpreterContextProcIter {
  IMP_InterpreterContextProcTableEntry *proc_table;
  ptrdiff_t index;
  ptrdiff_t len;
};

IMP_InterpreterContext *imp_interpreter_context_create(void) {
  IMP_InterpreterContext *context = malloc(sizeof(IMP_InterpreterContext));
  assert(context && "Memory allocation failed");
  context->var_table = NULL;
  context->proc_table = NULL;
  return context;
}

void imp_interpreter_context_destroy(IMP_InterpreterContext *context) {
  ptrdiff_t len = shlen(context->var_table);
  for (ptrdiff_t i = 0; i < len; ++i) free((char*)context->var_table[i].key);
  shfree(context->var_table);
}

int imp_interpreter_context_var_get(IMP_InterpreterContext *context, const char *name) {
  ptrdiff_t index = shgeti(context->var_table, name);
  if (index < 0) return 0;
  return context->var_table[index].value;
}

void imp_interpreter_context_var_set(IMP_InterpreterContext *context, const char *name, int value) {
  ptrdiff_t index = shgeti(context->var_table, name);
  if (index < 0) {
    if (value == 0) return;
    const char* key = strdup(name);
    assert(key && "Memory allocation failed");
    shput(context->var_table, key, value);
  } else {
    if (value == 0) {
      const char *key = context->var_table[index].key;
      shdel(context->var_table, name);
      free((char*)key);
      return;
    }
    context->var_table[index].value = value;
  }
}

const IMP_ASTNode *imp_interpreter_context_proc_get(IMP_InterpreterContext *context, const char *name) {
  ptrdiff_t index = shgeti(context->proc_table, name);
  if (index < 0) return NULL;
  return context->proc_table[index].value;
}

void imp_interpreter_context_proc_set(IMP_InterpreterContext *context, const char *name, const IMP_ASTNode *proc) {
  ptrdiff_t index = shgeti(context->proc_table, name);
  proc = imp_ast_clone(proc);
  if (proc) assert(proc->type == IMP_AST_NT_PROCDECL);
  if (index < 0) {
    if (proc == NULL) return;
    const char* key = strdup(name);
    assert(key && "Memory allocation failed");
    shput(context->proc_table, key, proc);
  } else {
    imp_ast_destroy((IMP_ASTNode*)context->proc_table[index].value);
    if (proc == NULL) {
      const char *key = context->proc_table[index].key;
      shdel(context->proc_table, name);
      free((char*)key);
      return;
    }
    context->proc_table[index].value = proc;
  }
}

IMP_InterpreterContextVarIter *imp_interpreter_context_var_iter_create(IMP_InterpreterContext *context) {
  IMP_InterpreterContextVarIter *iter = malloc(sizeof(IMP_InterpreterContextVarIter));
  assert(iter && "Memory allocation failed");
  iter->var_table = context->var_table;
  iter->index = 0;
  iter->len = shlen(context->var_table);
  return iter;
}

void imp_interpreter_context_var_iter_destroy(IMP_InterpreterContextVarIter *iter) {
  free(iter);
}

IMP_InterpreterContextVarTableEntry *imp_interpreter_context_var_iter_next(IMP_InterpreterContextVarIter *iter) {
  if (iter->index >= iter->len) return NULL;
  return &iter->var_table[iter->index++];
}

IMP_InterpreterContextProcIter *imp_interpreter_context_proc_iter_create(IMP_InterpreterContext *context) {
  IMP_InterpreterContextProcIter *iter = malloc(sizeof(IMP_InterpreterContextProcIter));
  assert(iter && "Memory allocation failed");
  iter->proc_table = context->proc_table;
  iter->index = 0;
  iter->len = shlen(context->proc_table);
  return iter;
}

void imp_interpreter_context_proc_iter_destroy(IMP_InterpreterContextProcIter *iter) {
  free(iter);
}

IMP_InterpreterContextProcTableEntry *imp_interpreter_context_proc_iter_next(IMP_InterpreterContextProcIter *iter) {
  if (iter->index >= iter->len) return NULL;
  return &iter->proc_table[iter->index++];
}