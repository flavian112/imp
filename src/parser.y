%{
#include <stdio.h>
#include "ast.h"

extern char *yytext;
extern int yylineno;
extern int yylex();

IMP_ASTNode *ast_root;

void yyerror(const char *s) { 
  fprintf(stderr, "Parse error at token \"%s\", line %d: %s\n", yytext, yylineno, s); 
}
%}


%union {
  int                    num;
  char                   *id;
  struct IMP_ASTNode     *node;
  struct IMP_ASTNodeList *node_list;
}

%start prog

%token <num> T_NUM
%token <id>  T_ID
%token       T_EQ T_NE T_LT T_LE T_GT T_GE
%token       T_TRUE T_FALSE
%left        T_OR
%left        T_AND
%token       T_NOT
%left        T_PLUS T_MINUS
%left        T_STAR
%right       T_UMINUS
%token       T_SKIP T_END T_IF T_THEN T_ELSE T_WHILE T_DO T_VAR T_IN T_PROC T_BEGIN
%token       T_ASSIGN
%token       T_LPAREN T_RPAREN T_COM T_SEM

%type <node> prog stm var aexp bexp procd procc
%type <node_list> args

%%

prog  : stm
        { ast_root = $1; }
      ;

stm   : T_SKIP
        { $$ = imp_ast_skip(); }
      | var T_ASSIGN aexp
        { $$ = imp_ast_assign($1, $3); }
      | T_LPAREN stm T_SEM stm T_RPAREN
        { $$ = imp_ast_seq($2, $4); }
      | stm T_SEM stm
        { $$ = imp_ast_seq($1, $3); }
      | stm T_SEM
        { $$ = $1; }
      | T_IF bexp T_THEN stm T_ELSE stm T_END
        { $$ = imp_ast_if($2, $4, $6); }
      | T_IF bexp T_THEN stm T_END
        { $$ = imp_ast_if($2, $4, imp_ast_skip()); }
      | T_WHILE bexp T_DO stm T_END
        { $$ = imp_ast_while($2, $4); }
      | T_VAR var T_ASSIGN aexp T_IN stm T_END
        { $$ = imp_ast_let($2, $4, $6); }
      | procd
        { $$ = $1; }
      | procc
        { $$ = $1; }
      ;

var   : T_ID
        { $$ = imp_ast_var($1); }
      ;

aexp  : aexp T_PLUS aexp
        { $$ = imp_ast_aop(IMP_AST_AOP_ADD, $1, $3); }
      | aexp T_MINUS aexp
        { $$ = imp_ast_aop(IMP_AST_AOP_SUB, $1, $3); }
      | aexp T_STAR aexp
        { $$ = imp_ast_aop(IMP_AST_AOP_MUL, $1, $3); }
      | T_MINUS aexp %prec T_UMINUS
        { $$ = imp_ast_aop(IMP_AST_AOP_SUB, imp_ast_int(0), $2); }
      | T_LPAREN aexp T_RPAREN
        { $$ = $2; }
      | var
        { $$ = $1; }
      | T_NUM
        { $$ = imp_ast_int($1); }
      ;

bexp  : bexp T_OR bexp
        { $$ = imp_ast_bop(IMP_AST_BOP_OR, $1, $3); }
      | bexp T_AND bexp
        { $$ = imp_ast_bop(IMP_AST_BOP_AND, $1, $3); }
      | T_NOT bexp
        { $$ = imp_ast_not($2); }
      | aexp T_EQ aexp
        { $$ = imp_ast_rop(IMP_AST_ROP_EQ, $1, $3); }
      | aexp T_NE aexp
        { $$ = imp_ast_rop(IMP_AST_ROP_NE, $1, $3); }
      | aexp T_LE aexp
        { $$ = imp_ast_rop(IMP_AST_ROP_LE, $1, $3); }
      | aexp T_LT aexp
        { $$ = imp_ast_rop(IMP_AST_ROP_LT, $1, $3); }
      | aexp T_GE aexp
        { $$ = imp_ast_rop(IMP_AST_ROP_GE, $1, $3); }
      | aexp T_GT aexp
        { $$ = imp_ast_rop(IMP_AST_ROP_GT, $1, $3); }
      | T_LPAREN bexp T_RPAREN
        { $$ = $2; }
      | T_TRUE
        { $$ = imp_ast_rop(IMP_AST_ROP_EQ, imp_ast_int(1), imp_ast_int(1)); }
      | T_FALSE
        { $$ = imp_ast_rop(IMP_AST_ROP_EQ, imp_ast_int(0), imp_ast_int(1)); }
      ;

args  : var
        { $$ = imp_ast_list($1, NULL); }
      | args T_COM var
        { $$ = imp_ast_list($3, $1); }
      ;

procd : T_PROC T_ID T_LPAREN args T_SEM args T_RPAREN T_BEGIN stm T_END
        { $$ = imp_ast_procdecl($2, $4, $6, $9); }
      ;

procc : T_ID T_LPAREN args T_SEM args T_RPAREN
        { $$ = imp_ast_proccall($1, $3, $5); }
      ;
%%
