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
    int val;
    char *var;
    struct { struct ASTNode *var, *aexp; } assign;
    struct {  struct ASTNode *exp1, *exp2; union { AOp aop; BOp bop; ROp rop; } op;} bin;
    struct ASTNode *bexp;
    struct { struct ASTNode *bexp, *s1, *s2; } cond;
    struct { struct ASTNode *s1, *s2; } seq;
  } u;
} ASTNode;

ASTNode *ast_skip(void);
ASTNode *ast_assign(ASTNode *var, ASTNode *aexp);
ASTNode *ast_seq(ASTNode *s1, ASTNode *s2);
ASTNode *ast_if(ASTNode *bexp, ASTNode *s1, ASTNode *s2);
ASTNode *ast_while(ASTNode *bexp, ASTNode *s);
ASTNode *ast_int(int val);
ASTNode *ast_var(char *var);
ASTNode *ast_aop(AOp, ASTNode *aexp1, ASTNode *aexp2);
ASTNode *ast_bop(BOp, ASTNode *bexp1, ASTNode *bexp2);
ASTNode *ast_not(ASTNode *bexp);
ASTNode *ast_rop(ROp, ASTNode *aexp1, ASTNode *aexp2);

void exec_stmt(ASTNode *n);
void free_ast(ASTNode *n);
void env_print(void);

#endif

