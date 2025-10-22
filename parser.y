%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
void yyerror(const char* s);

ASTNode* root = NULL;
%}

%union {
    int num;
    char* string;
    struct ASTNode* node;
}

%token <num> NUM
%token <string> ID
%token INT PRINT RETURN VOID

%type <node> program func_list func_decl param_list param stmt_list stmt decl assign print_stmt return_stmt
%type <node> expr array_decl array_assign array_2d_decl array_2d_assign arg_list
%type <string> type

%left '+' '-'
%left '*' '/'
%right '='

%%

program: func_list { root = $1; }
       ;

func_list: func_decl { $$ = $1; }
         | func_list func_decl { $$ = createFuncList($1, $2); }
         ;

func_decl: type ID '(' param_list ')' '{' stmt_list '}' 
         { $$ = createFuncDecl($1, $2, $4, $7); }
         | type ID '(' ')' '{' stmt_list '}' 
         { $$ = createFuncDecl($1, $2, NULL, $6); }
         ;

type: INT { $$ = "int"; }
    | VOID { $$ = "void"; }
    ;

param_list: param { $$ = $1; }
          | param_list ',' param { $$ = createParamList($1, $3); }
          ;

param: INT ID { $$ = createParam("int", $2); }
     ;

stmt_list: stmt { $$ = $1; }
         | stmt_list stmt { $$ = createStmtList($1, $2); }
         ;

stmt: decl
    | assign
    | print_stmt
    | return_stmt
    | array_decl
    | array_assign
    | array_2d_decl
    | array_2d_assign
    ;

decl: INT ID ';' { $$ = createDecl($2); }
    ;

array_decl: INT ID '[' NUM ']' ';' { $$ = createArrayDecl($2, $4); }
          ;

array_2d_decl: INT ID '[' NUM ']' '[' NUM ']' ';' { $$ = createArray2DDecl($2, $4, $7); }
             ;

assign: ID '=' expr ';' { $$ = createAssign($1, $3); }
      ;

array_assign: ID '[' expr ']' '=' expr ';' { $$ = createArrayAssign($1, $3, $6); }
            ;

array_2d_assign: ID '[' expr ']' '[' expr ']' '=' expr ';' 
               { $$ = createArray2DAssign($1, $3, $6, $9); }
               ;

print_stmt: PRINT '(' expr ')' ';' { $$ = createPrint($3); }
          ;

return_stmt: RETURN expr ';' { $$ = createReturn($2); }
           | RETURN ';' { $$ = createReturn(NULL); }
           ;

arg_list: expr { $$ = createArgList($1, NULL); }
        | arg_list ',' expr { $$ = createArgList($3, $1); }
        ;

expr: expr '+' expr { $$ = createBinOp('+', $1, $3); }
    | expr '-' expr { $$ = createBinOp('-', $1, $3); }
    | expr '*' expr { $$ = createBinOp('*', $1, $3); }
    | '(' expr ')' { $$ = $2; }
    | NUM { $$ = createNum($1); }
    | ID { $$ = createVar($1); }
    | ID '[' expr ']' { $$ = createArrayAccess($1, $3); }
    | ID '[' expr ']' '[' expr ']' { $$ = createArray2DAccess($1, $3, $6); }
    | ID '(' arg_list ')' { $$ = createFuncCall($1, $3); }
    | ID '(' ')' { $$ = createFuncCall($1, NULL); }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Syntax Error: %s\n", s);
}