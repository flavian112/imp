#ifndef AST_H
#define AST_H

typedef enum {
  NT_SKIP, NT_ASSIGN, NT_SEQ, NT_IF, NT_WHILE,
  NT_INT, NT_VAR, NT_AOP, NT_BOP, NT_NOT, NT_ROP
} NodeType;

typedef enum { AOP_ADD, AOP_SUB, AOP_MUL } AOp;
typedef enum { BOP_AND, BOP_OR } BOp;
typedef enum { ROP_EQ, ROP_NE, ROP_LT, ROP_LE, ROP_GT, ROP_GE } ROp;

typedef struct ASTNode {
  NodeType type;
  union {
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
  } u;
} ASTNode;

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

typedef struct Env {
  char *name;
  int val;
  struct Env *next;
} Env;

void exec_stmt(Env **env, ASTNode *node);
void free_ast(ASTNode *node);
void env_print(Env *env);

#endif

