%{
#include <stdio.h>
#include "ast.h"

extern char *yytext;
extern int yylineno;
extern int yylex();

ASTNode *ast_root;

void yyerror(const char *s) { 
  fprintf(stderr, "Parse error at token \"%s\", line %d: %s\n", yytext, yylineno, s); 
}
%}


%union {
  int            num;
  char           *id;
  struct ASTNode *node;
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
%token       T_END T_IF T_THEN T_ELSE T_WHILE T_DO T_VAR T_IN
%token       T_SKIP
%token       T_ASSIGN
%token       T_LPAREN T_RPAREN
%token       T_SEM

%type <node> prog stm var aexp bexp

%%

prog  : stm
        { ast_root = $1; }
      ;

stm   : T_SKIP
        { $$ = ast_skip(); }
      | var T_ASSIGN aexp
        { $$ = ast_assign($1, $3); }
      | T_LPAREN stm T_SEM stm T_RPAREN
        { $$ = ast_seq($2, $4); }
      | stm T_SEM stm
        { $$ = ast_seq($1, $3); }
      | stm T_SEM
        { $$ = $1; }
      | T_IF bexp T_THEN stm T_ELSE stm T_END
        { $$ = ast_if($2, $4, $6); }
      | T_IF bexp T_THEN stm T_END
        { $$ = ast_if($2, $4, ast_skip()); }
      | T_WHILE bexp T_DO stm T_END
        { $$ = ast_while($2, $4); }
      | T_VAR var T_ASSIGN aexp T_IN stm T_END
        { $$ = ast_let($2, $4, $6); }
      ;

var   : T_ID
        { $$ = ast_var($1); }
      ;

aexp  : aexp T_PLUS aexp
        { $$ = ast_aop(AOP_ADD, $1, $3); }
      | aexp T_MINUS aexp
        { $$ = ast_aop(AOP_SUB, $1, $3); }
      | aexp T_STAR aexp
        { $$ = ast_aop(AOP_MUL, $1, $3); }
      | T_MINUS aexp %prec T_UMINUS
        { $$ = ast_aop(AOP_SUB, ast_int(0), $2); }
      | T_LPAREN aexp T_RPAREN
        { $$ = $2; }
      | var
        { $$ = $1; }
      | T_NUM
        { $$ = ast_int($1); }
      ;

bexp  : bexp T_OR bexp
        { $$ = ast_bop(BOP_OR, $1, $3); }
      | bexp T_AND bexp
        { $$ = ast_bop(BOP_AND, $1, $3); }
      | T_NOT bexp
        { $$ = ast_not($2); }
      | aexp T_EQ aexp
        { $$ = ast_rop(ROP_EQ, $1, $3); }
      | aexp T_NE aexp
        { $$ = ast_rop(ROP_NE, $1, $3); }
      | aexp T_LE aexp
        { $$ = ast_rop(ROP_LE, $1, $3); }
      | aexp T_LT aexp
        { $$ = ast_rop(ROP_LT, $1, $3); }
      | aexp T_GE aexp
        { $$ = ast_rop(ROP_GE, $1, $3); }
      | aexp T_GT aexp
        { $$ = ast_rop(ROP_GT, $1, $3); }
      | T_LPAREN bexp T_RPAREN
        { $$ = $2; }
      | T_TRUE
        { $$ = ast_rop(ROP_EQ, ast_int(1), ast_int(1)); }
      | T_FALSE
        { $$ = ast_rop(ROP_EQ, ast_int(0), ast_int(1)); }
      ;

%%
