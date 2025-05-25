#ifndef IMP_DRIVER_H
#define IMP_DRIVER_H

#include "interpreter_context.h"

int imp_driver_interpret_file (IMP_InterpreterContext *context, const char *path);
int imp_driver_interpret_str (IMP_InterpreterContext *context, const char *str);
int imp_driver_print_ast_file (const char *path);

void imp_driver_print_var_table(IMP_InterpreterContext *context);
void imp_driver_print_proc_table(IMP_InterpreterContext *context);

#endif /* IMP_DRIVER_H */