#ifndef IMP_INTERPRETER_H
#define IMP_INTERPRETER_H


/**
 * @file interpreter.h
 * @brief Interpreter for evaluating IMP language programs.
 *
 * @author Flavian Kaufmann
 */


#include "ast.h"
#include "interpreter_context.h"


/**
 * Evaluates an AST node within a given context.
 *
 * @param context The interpreter context.
 * @param node AST node to evaluate.
 * @return Status code or result of evaluation. (0 for success, non-zero for error).
 *
 * @note The interpreter does neither take ownership of the context nor the AST node.
 */
int imp_interpreter_interpret_ast(IMP_InterpreterContext *context, const IMP_ASTNode *node);



#endif /* IMP_INTERPRETER_H */