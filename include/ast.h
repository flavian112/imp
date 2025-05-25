#ifndef IMP_AST_H
#define IMP_AST_H

/**
 * @file ast.h
 * @brief Defines the Abstract Syntax Tree (AST) structures and operations for the IMP language.
 *
 * Provides data structures, enums, and functions for creating, managing, cloning, and freeing AST nodes.
 *
 * @author Flavian Kaufmann
 */


/** AST node types for IMP language. */
typedef enum {
  IMP_AST_NT_SKIP,        /**< No-operation (skip) */
  IMP_AST_NT_ASSIGN,      /**< Assignment statement */
  IMP_AST_NT_SEQ,         /**< Sequential composition of statements */
  IMP_AST_NT_IF,          /**< Conditional (if-then-else) statement */
  IMP_AST_NT_WHILE,       /**< While loop statement */
  IMP_AST_NT_INT,         /**< Integer literal */
  IMP_AST_NT_VAR,         /**< Variable reference */
  IMP_AST_NT_AOP,         /**< Arithmetic operation */
  IMP_AST_NT_BOP,         /**< Boolean operation */
  IMP_AST_NT_NOT,         /**< Boolean negation */
  IMP_AST_NT_ROP,         /**< Relational operation */
  IMP_AST_NT_LET,         /**< Local variable declaration (let-in-end) */
  IMP_AST_NT_PROCDECL,    /**< Procedure declaration */
  IMP_AST_NT_PROCCALL     /**< Procedure call */
} IMP_ASTNodeType;

/** Arithmetic operators. */
typedef enum {
  IMP_AST_AOP_ADD, /**< Addition (+) */
  IMP_AST_AOP_SUB, /**< Subtraction (-) */
  IMP_AST_AOP_MUL  /**< Multiplication (*) */
} IMP_ASTArithmeticOperator;

/** Boolean operators. */
typedef enum {
  IMP_AST_BOP_AND, /**< Logical AND */
  IMP_AST_BOP_OR   /**< Logical OR */
} IMP_ASTBooleanOperator;

/** Relational operators. */
typedef enum {
  IMP_AST_ROP_EQ, /**< Equal (==) */
  IMP_AST_ROP_NE, /**< Not equal (!=) */
  IMP_AST_ROP_LT, /**< Less than (<) */
  IMP_AST_ROP_LE, /**< Less or equal (<=) */
  IMP_AST_ROP_GT, /**< Greater than (>) */
  IMP_AST_ROP_GE  /**< Greater or equal (>=) */
} IMP_ASTRelationalOperator;

/** Forward declaration for linked-list structure. */
struct IMP_ASTNodeList;

/** Abstract Syntax Tree node structure. */
typedef struct IMP_ASTNode {
  IMP_ASTNodeType type; /**< Type of the AST node. */

  union {
    struct { struct IMP_ASTNode *var, *aexpr; } assign;
    struct { struct IMP_ASTNode *fst_stmt, *snd_stmt; } seq;
    struct { struct IMP_ASTNode *cond_bexpr, *then_stmt, *else_stmt; } if_stmt;
    struct { struct IMP_ASTNode *cond_bexpr, *body_stmt; } while_stmt;
    struct { int val; } integer;
    struct { char *name; } variable;
    struct { IMP_ASTArithmeticOperator aopr; struct IMP_ASTNode *l_aexpr, *r_aexpr; } arith_op;
    struct { IMP_ASTBooleanOperator bopr; struct IMP_ASTNode *l_bexpr, *r_bexpr; } bool_op;
    struct { struct IMP_ASTNode *bexpr; } bool_not;
    struct { IMP_ASTRelationalOperator ropr; struct IMP_ASTNode *l_aexpr, *r_aexpr; } rel_op;
    struct { struct IMP_ASTNode *var, *aexpr, *body_stmt; } let_stmt;
    struct { char *name; struct IMP_ASTNodeList *val_args, *var_args; struct IMP_ASTNode *body_stmt; } proc_decl;
    struct { char *name; struct IMP_ASTNodeList *val_args, *var_args; } proc_call;
  } data;
} IMP_ASTNode;

/** Linked list of AST nodes, primarily for arguments in procedures. */
typedef struct IMP_ASTNodeList {
  IMP_ASTNode *node;                  /**< AST node reference. */
  struct IMP_ASTNodeList *next;       /**< Pointer to the next node in the list. */
} IMP_ASTNodeList;

/* === AST Node Creation Functions === */

/** Creates a 'skip' node (no-operation). */
IMP_ASTNode *imp_ast_skip(void);

/** Creates an assignment node. */
IMP_ASTNode *imp_ast_assign(IMP_ASTNode *var, IMP_ASTNode *aexpr);

/** Creates a sequence node (statement sequencing). */
IMP_ASTNode *imp_ast_seq(IMP_ASTNode *fst_stmt, IMP_ASTNode *snd_stmt);

/** Creates an if-then-else node. */
IMP_ASTNode *imp_ast_if(IMP_ASTNode *cond_bexpr, IMP_ASTNode *then_stmt, IMP_ASTNode *else_stmt);

/** Creates a while loop node. */
IMP_ASTNode *imp_ast_while(IMP_ASTNode *cond_bexpr, IMP_ASTNode *body_stmt);

/** Creates an integer literal node. */
IMP_ASTNode *imp_ast_int(int val);

/** Creates a variable reference node. (Name is copied internally.) */
IMP_ASTNode *imp_ast_var(const char *name);

/** Creates an arithmetic operation node. */
IMP_ASTNode *imp_ast_aop(IMP_ASTArithmeticOperator aopr, IMP_ASTNode *l_aexpr, IMP_ASTNode *r_aexpr);

/** Creates a boolean operation node. */
IMP_ASTNode *imp_ast_bop(IMP_ASTBooleanOperator bopr, IMP_ASTNode *l_bexpr, IMP_ASTNode *r_bexpr);

/** Creates a boolean negation node. */
IMP_ASTNode *imp_ast_not(IMP_ASTNode *bexpr);

/** Creates a relational operation node. */
IMP_ASTNode *imp_ast_rop(IMP_ASTRelationalOperator ropr, IMP_ASTNode *l_aexpr, IMP_ASTNode *r_aexpr);

/** Creates a let-in-end (local variable declaration) node. */
IMP_ASTNode *imp_ast_let(IMP_ASTNode *var, IMP_ASTNode *aexpr, IMP_ASTNode *body_stmt);

/** Creates a procedure declaration node. (Name is copied internally.) */
IMP_ASTNode *imp_ast_procdecl(const char *name, IMP_ASTNodeList *val_args, IMP_ASTNodeList *var_args, IMP_ASTNode *body_stmt);

/** Creates a procedure call node. (Name is copied internally.) */
IMP_ASTNode *imp_ast_proccall(const char *name, IMP_ASTNodeList *val_args, IMP_ASTNodeList *var_args);

/* === AST Utility Functions === */

/**
 * Creates a deep copy of the given AST node and all its sub-nodes.
 *
 * @param node Node to clone.
 * @return Pointer to cloned node; must be freed by the caller.
 */
IMP_ASTNode *imp_ast_clone(const IMP_ASTNode *node);

/**
 * Frees an AST node and recursively all its sub-nodes.
 *
 * @param node Node to free.
 */
void imp_ast_destroy(IMP_ASTNode *node);

/**
 * Creates a new linked list of AST nodes.
 *
 * @param node Node to prepend to the list.
 * @param list Existing list or NULL if starting a new one.
 * @return New head of the list.
 *
 * @note Ownership of the list nodes is transferred; freed automatically when parent AST node is freed.
 */
IMP_ASTNodeList *imp_ast_list(IMP_ASTNode *node, IMP_ASTNodeList *list);

/**
 * Frees a linked list of AST nodes. Typically not required directly, as it's freed with the parent AST node.
 *
 * @param list List to free.
 */
void imp_ast_list_destroy(IMP_ASTNodeList *list);

#endif /* IMP_AST_H */