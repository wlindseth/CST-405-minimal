%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

void yyerror(const char* s);
%}

%option yylineno

%%

[ \t\n]+              { /* Skip whitespace */ }
"//".*                { /* Skip single-line comments */ }

"int"                 { return INT; }
"print"               { return PRINT; }
"return"              { return RETURN; }
"void"                { return VOID; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.string = strdup(yytext); return ID; }
[0-9]+                { yylval.num = atoi(yytext); return NUM; }

"+"                   { return '+'; }
"-"                   { return '-'; }
"*"                   { return '*'; }
"/"                   { return '/'; }
"="                   { return '='; }
";"                   { return ';'; }
","                   { return ','; }
"("                   { return '('; }
")"                   { return ')'; }
"{"                   { return '{'; }
"}"                   { return '}'; }
"["                   { return '['; }
"]"                   { return ']'; }

.                     { 
                        fprintf(stderr, "Lexical Error: Unknown character '%s'\n", yytext); 
                      }

%%

int yywrap() {
    return 1;
}