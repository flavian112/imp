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
  int            op;
  struct ASTNode *node;
}

%start program

%token <id>  TOKEN_IDENTIFIER
%token <num> TOKEN_NUMERAL
%token       TOKEN_ASSIGN
%token       TOKEN_LEFT_PARENTHESIS TOKEN_RIGHT_PARENTHESIS
%token       TOKEN_SEMICOLON
%token       TOKEN_SKIP
%token       TOKEN_IF TOKEN_THEN TOKEN_ELSE TOKEN_END TOKEN_WHILE TOKEN_DO
%token       TOKEN_PLUS TOKEN_MINUS TOKEN_MULTIPLY
%token       TOKEN_NOT TOKEN_OR TOKEN_AND
%token       TOKEN_EQUALS TOKEN_NOT_EQUALS TOKEN_LESS_THAN TOKEN_LESS_EQUAL TOKEN_GREATER_THAN TOKEN_GREATER_EQUAL

%type <node> program statement variable arithmetic_expression boolean_expression
%type <op>   arithmetic_operation boolean_operation relational_operation


%%
  
program               : statement
                        { ast_root = $1; }
                      ;

statement             : TOKEN_SKIP
                        { $$ = ast_skip(); }
                      | variable TOKEN_ASSIGN arithmetic_expression
                        { $$ = ast_assign($1, $3); }
                      | TOKEN_LEFT_PARENTHESIS statement TOKEN_SEMICOLON statement TOKEN_RIGHT_PARENTHESIS
                        { $$ = ast_seq($2, $4); }
                      | TOKEN_IF boolean_expression TOKEN_THEN statement TOKEN_ELSE statement TOKEN_END
                        { $$ = ast_if($2, $4, $6); }
                      | TOKEN_WHILE boolean_expression TOKEN_DO statement TOKEN_END
                        { $$ = ast_while($2, $4); }
                      ;

variable              : TOKEN_IDENTIFIER
                        { $$ = ast_var($1); }
                      ;

arithmetic_expression : TOKEN_LEFT_PARENTHESIS arithmetic_expression arithmetic_operation arithmetic_expression TOKEN_RIGHT_PARENTHESIS
                        { $$ = ast_aop($3, $2, $4); }
                      | variable
                        { $$ = $1; }
                      | TOKEN_NUMERAL
                        { $$ = ast_int($1); }
                      ;

arithmetic_operation  : TOKEN_PLUS
                        { $$ = AOP_ADD; }
                      | TOKEN_MINUS
                        { $$ = AOP_SUB; }
                      | TOKEN_MULTIPLY
                        { $$ = AOP_MUL; }
                      ;

boolean_expression    : TOKEN_LEFT_PARENTHESIS boolean_expression boolean_operation boolean_expression TOKEN_RIGHT_PARENTHESIS
                        { $$ = ast_bop($3, $2, $4); }
                      | TOKEN_NOT boolean_expression
                        { $$ = ast_not($2); }
                      | arithmetic_expression relational_operation arithmetic_expression
                        { $$ = ast_rop($2, $1, $3); }
                      ;

boolean_operation     : TOKEN_OR
                        { $$ = BOP_OR; }
                      | TOKEN_AND
                        { $$ = BOP_AND; }
                      ;

relational_operation  : TOKEN_EQUALS
                        { $$ = ROP_EQ; }
                      | TOKEN_NOT_EQUALS
                        { $$ = ROP_NE; }
                      | TOKEN_LESS_THAN
                        { $$ = ROP_LT; }
                      | TOKEN_LESS_EQUAL
                        { $$ = ROP_LE; }
                      | TOKEN_GREATER_THAN
                        { $$ = ROP_GT; }
                      | TOKEN_GREATER_EQUAL
                        { $$ = ROP_GE; }
                      ;

%%
