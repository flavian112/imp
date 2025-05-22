#ifndef AST_H
#define AST_H


/* AST Node Types */
typedef enum {
  NT_SKIP,     /* nop */
  NT_ASSIGN,   /* assignment */
  NT_SEQ,      /* sequential composition */
  NT_IF,       /* if-then-else block */
  NT_WHILE,    /* while loop */
  NT_INT,      /* integer constant */
  NT_VAR,      /* integer variable */
  NT_AOP,      /* arithmetic operation */
  NT_BOP,      /* boolean operation */
  NT_NOT,      /* boolean not */
  NT_ROP,      /* relational operation */
  NT_LET,      /* let-in-end block (i.e. local variable) */
  NT_PROCDECL, /* procedure declaration */
  NT_PROCCALL  /* procedure call */
} NodeType;

/* Arithmetic Operations */
typedef enum { 
  AOP_ADD, /* addition */
  AOP_SUB, /* subtraction */
  AOP_MUL  /* multiplication */
} AOp;

/* Boolean Operations */
typedef enum { 
  BOP_AND, /* conjunction */
  BOP_OR   /* disjunction */
} BOp;

/* Relational Operations */
typedef enum { 
  ROP_EQ, /* equals */
  ROP_NE, /* not equals */
  ROP_LT, /* less than */
  ROP_LE, /* less or equals */
  ROP_GT, /* greater than */
  ROP_GE  /* greater or equals */
} ROp;

/* AST Node */
typedef struct ASTNode {
  NodeType type;
  union { /* specific data for each node type */
    struct { struct ASTNode *var, *aexp; } d_assign;
    struct { struct ASTNode *stm1, *stm2; } d_seq;
    struct { struct ASTNode *bexp, *stm1, *stm2; } d_if;
    struct { struct ASTNode *bexp, *stm; } d_while;
    struct { int val; } d_int ;
    struct { char *name; } d_var;
    struct { AOp aop; struct ASTNode *aexp1, *aexp2; } d_aop;
    struct { BOp bop; struct ASTNode *bexp1, *bexp2; } d_bop;
    struct { struct ASTNode *bexp; } d_not;
    struct { ROp rop; struct ASTNode *aexp1, *aexp2; } d_rop;
    struct { struct ASTNode *var, *aexp, *stm; } d_let;
    struct { char *name; struct ASTNodeList *args,  *vargs; struct ASTNode *stm; } d_procdecl;
    struct { char *name; struct ASTNodeList *args,  *vargs; } d_proccall;
  } u;
} ASTNode;

/* AST Node List (i.e. linked list of AST Nodes) */
typedef struct ASTNodeList {
  ASTNode *node;
  struct ASTNodeList *next;
} ASTNodeList;


/* 
  Creates new node of specific type.
    The caller is responsible for freeing the top-most node, by calling ast_free.
    Any strings passed to the node are copied (i.e. strdup) and will also automatically be freed.
*/
ASTNode *ast_skip(void);
ASTNode *ast_assign(ASTNode *var, ASTNode *aexp);
ASTNode *ast_seq(ASTNode *stm1, ASTNode *stm2);
ASTNode *ast_if(ASTNode *bexp, ASTNode *stm1, ASTNode *stm2);
ASTNode *ast_while(ASTNode *bexp, ASTNode *stm);
ASTNode *ast_int(int val);
ASTNode *ast_var(char *name);
ASTNode *ast_aop(AOp aop, ASTNode *aexp1, ASTNode *aexp2);
ASTNode *ast_bop(BOp bop, ASTNode *bexp1, ASTNode *bexp2);
ASTNode *ast_not(ASTNode *bexp);
ASTNode *ast_rop(ROp rop, ASTNode *aexp1, ASTNode *aexp2);
ASTNode *ast_let(ASTNode *var, ASTNode *aexp, ASTNode *stm);
ASTNode *ast_procdecl(const char *name, ASTNodeList *args, ASTNodeList *vargs, ASTNode *stm);
ASTNode *ast_proccall(const char *name, ASTNodeList *args, ASTNodeList *vargs);

/* 
  Creates node list, 
    Must be used in conjunction with a node that takes a list (i.e. ast_procdecl).
    Will automatically be freed when the parent node is freed.
  */
ASTNodeList *ast_node_list(ASTNode *node, ASTNodeList *list);

/* 
  Creates a deep copy of a node and it's sub nodes (recursively).
    caller is responsible for freeing top-most node.
*/
ASTNode *ast_clone(ASTNode *node);

/* Recursively frees node and all sub nodes */
void ast_free(ASTNode *node);


#endif