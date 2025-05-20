%option noyywrap yylineno

%{
#include "parser.tab.h"
%}

DIGIT       [0-9]
IDENT       [A-Za-z][A-Za-z0-9]*
WHITESPACE  [ \t\r\n]+

%%

"skip"          { return TOKEN_SKIP; }
"if"            { return TOKEN_IF; }
"then"          { return TOKEN_THEN; }
"else"          { return TOKEN_ELSE; }
"end"           { return TOKEN_END; }
"while"         { return TOKEN_WHILE; }
"do"            { return TOKEN_DO; }

"("             { return TOKEN_LEFT_PARENTHESIS; }
")"             { return TOKEN_RIGHT_PARENTHESIS; }
";"             { return TOKEN_SEMICOLON; }
":="            { return TOKEN_ASSIGN; }

"+"             { return TOKEN_PLUS; }
"-"             { return TOKEN_MINUS; }
"*"             { return TOKEN_MULTIPLY; }

"or"            { return TOKEN_OR; }
"and"           { return TOKEN_AND; }
"not"           { return TOKEN_NOT; }

"="             { return TOKEN_EQUALS; }
"#"             { return TOKEN_NOT_EQUALS; }
"<="            { return TOKEN_LESS_EQUAL; }
"<"             { return TOKEN_LESS_THAN; }
">="            { return TOKEN_GREATER_EQUAL; }
">"             { return TOKEN_GREATER_THAN; }

{DIGIT}+        { yylval.num = atoi(yytext); return TOKEN_NUMERAL; }
{IDENT}         { yylval.id = strdup(yytext); return TOKEN_IDENTIFIER; }

{WHITESPACE}    { }
.               { fprintf(stderr, "Unknown char: %s\n", yytext); }

%%
