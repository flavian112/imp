%option noyywrap yylineno

%{
#include "parser.tab.h"
%}

DIGIT       [0-9]
IDENT       [A-Za-z][A-Za-z0-9]*
WHITESPACE  [ \t\r\n]+

%%

"skip"          { return T_SKIP; }
"if"            { return T_IF; }
"then"          { return T_THEN; }
"else"          { return T_ELSE; }
"end"           { return T_END; }
"while"         { return T_WHILE; }
"do"            { return T_DO; }
"var"           { return T_VAR; }
"in"            { return T_IN; }

"("             { return T_LPAREN; }
")"             { return T_RPAREN; }
";"             { return T_SEM; }
":="            { return T_ASSIGN; }

"+"             { return T_PLUS; }
"-"             { return T_MINUS; }
"*"             { return T_STAR; }

"or"            { return T_OR; }
"and"           { return T_AND; }
"not"           { return T_NOT; }

"="             { return T_EQ; }
"#"             { return T_NE; }
"<="            { return T_LE; }
"<"             { return T_LT; }
">="            { return T_GE; }
">"             { return T_GT; }

"true"          { return T_TRUE; }
"false"         { return T_FALSE; }

{DIGIT}+        { yylval.num = atoi(yytext); return T_NUM; }
{IDENT}         { yylval.id = strdup(yytext); return T_ID; }

{WHITESPACE}    { }
.               { fprintf(stderr, "Unknown char: %s\n", yytext); }

%%
