#ifndef IMP_INTERPRETER_CONTEXT_H
#define IMP_INTERPRETER_CONTEXT_H

/**
 * @file interpreter_context.h
 * @brief Defines the context structure for the IMP interpreter.
 *
 * Provides the interface for managing the interpreter state, specifically
 * the variable and procedure environments.
 *
 * Author: Flavian Kaufmann
 */

#include "ast.h"

/**
 * @brief Opaque type representing the interpreter context.
 */
typedef struct IMP_InterpreterContext IMP_InterpreterContext;

/**
 * @brief Opaque type for iterating over variable entries in the context.
 */
typedef struct IMP_InterpreterContextVarIter IMP_InterpreterContextVarIter;

/**
 * @brief Opaque type for iterating over procedure entries in the context.
 */
typedef struct IMP_InterpreterContextProcIter IMP_InterpreterContextProcIter;

/**
 * @brief A single variable entry in the interpreter's environment.
 */
typedef struct IMP_InterpreterContextVarTableEntry {
  const char *key;  /**< The variable name (identifier). */
  int value;        /**< The value associated with the variable. */
} IMP_InterpreterContextVarTableEntry;

/**
 * @brief A single procedure entry in the interpreter's environment.
 */
typedef struct IMP_InterpreterContextProcTableEntry {
  const char *key;             /**< The procedure name (identifier). */
  const IMP_ASTNode *value;    /**< The AST node representing the procedure declaration. */
} IMP_InterpreterContextProcTableEntry;

/**
 * @brief Creates and initializes a new interpreter context.
 * 
 * @return A pointer to the newly created context.
 */
IMP_InterpreterContext *imp_interpreter_context_create(void);

/**
 * @brief Frees all memory associated with the interpreter context.
 * 
 * @param context The context to destroy.
 */
void imp_interpreter_context_destroy(IMP_InterpreterContext *context);

/**
 * @brief Retrieves the value of a variable from the context.
 * 
 * @param context The interpreter context.
 * @param name The name of the variable. (Is copied internally.)
 * @return The value of the variable. Returns 0 if the variable is not found.
 */
int imp_interpreter_context_var_get(IMP_InterpreterContext *context, const char *name);

/**
 * @brief Sets or updates the value of a variable in the context.
 * 
 * @param context The interpreter context.
 * @param name The name of the variable. (Is copied internally.)
 * @param value The value to assign. (If value is 0, the variable is removed from the context.)
 */
void imp_interpreter_context_var_set(IMP_InterpreterContext *context, const char *name, int value);

/**
 * @brief Retrieves the AST node for a procedure from the context.
 * 
 * @param context The interpreter context.
 * @param name The name of the procedure. (Is copied internally.)
 * @return A pointer to the procedure's AST node, or NULL if not found.
 */
const IMP_ASTNode *imp_interpreter_context_proc_get(IMP_InterpreterContext *context, const char *name);

/**
 * @brief Adds or updates a procedure in the context.
 * 
 * @param context The interpreter context.
 * @param name The name of the procedure. (Is copied internally.)
 * @param proc The AST node representing the procedure body. (Is cloned internally.)
 */
void imp_interpreter_context_proc_set(IMP_InterpreterContext *context, const char *name, const IMP_ASTNode *proc);

/**
 * @brief Creates an iterator over the variables in the context. (Is invalid if the variable table is modified.)
 * 
 * @param context The interpreter context.
 * @return A pointer to the variable iterator.
 */
IMP_InterpreterContextVarIter *imp_interpreter_context_var_iter_create(IMP_InterpreterContext *context);

/**
 * @brief Destroys a variable iterator.
 * 
 * @param iter The iterator to destroy.
 */
void imp_interpreter_context_var_iter_destroy(IMP_InterpreterContextVarIter *iter);

/**
 * @brief Retrieves the next variable entry from the iterator.
 * 
 * @param iter The variable iterator.
 * @return A pointer to the next variable entry, or NULL if end is reached.
 */
IMP_InterpreterContextVarTableEntry *imp_interpreter_context_var_iter_next(IMP_InterpreterContextVarIter *iter);

/**
 * @brief Creates an iterator over the procedures in the context. (Is invalid if the procedure table is modified.)
 * 
 * @param context The interpreter context.
 * @return A pointer to the procedure iterator.
 */
IMP_InterpreterContextProcIter *imp_interpreter_context_proc_iter_create(IMP_InterpreterContext *context);

/**
 * @brief Destroys a procedure iterator.
 * 
 * @param iter The iterator to destroy.
 */
void imp_interpreter_context_proc_iter_destroy(IMP_InterpreterContextProcIter *iter);

/**
 * @brief Retrieves the next procedure entry from the iterator.
 * 
 * @param iter The procedure iterator.
 * @return A pointer to the next procedure entry, or NULL if end is reached.
 */
IMP_InterpreterContextProcTableEntry *imp_interpreter_context_proc_iter_next(IMP_InterpreterContextProcIter *iter);

#endif /* IMP_INTERPRETER_CONTEXT_H */