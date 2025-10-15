%{
/* SYNTAX ANALYZER (PARSER)
 * This is the second phase of compilation - checking grammar rules
 * Bison generates a parser that builds an Abstract Syntax Tree (AST)
 * The parser uses tokens from the scanner to verify syntax is correct
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* External declarations for lexer interface */
extern int yylex();      /* Get next token from scanner */
extern int yyparse();    /* Parse the entire input */
extern FILE* yyin;       /* Input file handle */

void yyerror(const char* s);  /* Error handling function */
ASTNode* root = NULL;          /* Root of the Abstract Syntax Tree */
%}

/* SEMANTIC VALUES UNION
 * Defines possible types for tokens and grammar symbols
 * This allows different grammar rules to return different data types
 */
%union {
    int num;                /* For integer literals */
    char* str;              /* For identifiers */
    struct ASTNode* node;   /* For AST nodes */
}

/* TOKEN DECLARATIONS with their semantic value types */
%token <num> NUM        /* Number token carries an integer value */
%token <str> ID         /* Identifier token carries a string */
%token INT PRINT        /* Keywords have no semantic value */

/* NON-TERMINAL TYPES - Define what type each grammar rule returns */
%type <node> program stmt_list stmt decl assign expr print_stmt
%type <node> func_list func_decl param_list param return_stmt arg_list /* Added for clarity */
%type <str> type /* This is the missing declaration */

/* OPERATOR PRECEDENCE AND ASSOCIATIVITY */
%left '+'  /* Addition is left-associative: a+b+c = (a+b)+c */

%%

/* GRAMMAR RULES - Define the structure of our language */

/* PROGRAM RULE - Entry point of our grammar */
program:
    func_list { $$ = $1; }
    ;

func_list:
    func_decl { $$ = $1; }
    | func_list func_decl { $$ = createFuncList($1, $2); }
    ;

func_decl:
    type ID '(' param_list ')' '{' stmt_list '}' {
        $$ = createFuncDecl($1, $2, $4, $7);
    }
    | type ID '(' ')' '{' stmt_list '}' {
        $$ = createFuncDecl($1, $2, NULL, $6);
    }
    ;

param_list:
    param { $$ = $1; }
    | param_list ',' param { $$ = createParamList($1, $3); }
    ;

param:
    type ID { $$ = createParam($1, $2); }
    ;

type:
    INT { $$ = "int"; }
    | VOID { $$ = "void"; }
    ;

/* STATEMENT LIST - Handles multiple statements */
stmt_list:
    stmt { 
        /* Base case: single statement */
        $$ = $1;  /* Pass the statement up as-is */
    }
    | stmt_list stmt { 
        /* Recursive case: list followed by another statement */
        $$ = createStmtList($1, $2);  /* Build linked list of statements */
    }
    ;

/* STATEMENT TYPES - The three kinds of statements we support */
stmt:
    decl        /* Variable declaration */
    | assign    /* Assignment statement */
    | print_stmt /* Print statement */
    | return_stmt
    ;

return_stmt:
    RETURN expr ';' { $$ = createReturn($2); }
    | RETURN ';' { $$ = createReturn(NULL); }
    ;

/* DECLARATION RULE - "int x;" or "int arr[10];" */
decl:
    INT ID ';' { 
        /* Create declaration node for a regular variable */
        $$ = createDecl($2);
        free($2);
    }
    | INT ID '[' NUM ']' ';' { 
        /* Create declaration node for an array */
        $$ = createArrayDecl($2, $4); /* NOTE: New AST function needed */
        free($2);
    }
    | INT ID '[' NUM ']' '[' NUM ']' ';'
    { 
        $$ = createArray2DDecl($2, $4, $7); 
    }
    ;
/* ASSIGNMENT RULE - "x = expr;" or "arr[i] = expr;" */
assign:
    ID '=' expr ';' { 
        /* Create assignment node for a regular variable */
        $$ = createAssign($1, $3);
        free($1);
    }
    | ID '[' expr ']' '=' expr ';' { 
        /* Create assignment node for an array element */
        $$ = createArrayAssign($1, $3, $6); /* NOTE: New AST function needed */
        free($1);
    }
    | ID '[' expr ']' '[' expr ']' '=' expr ';'
    { 
        $$ = createArray2DAssign($1, $3, $6, $9); 
    }
    ;
    
/* EXPRESSION RULES - Build expression trees */
expr:
    NUM { 
        $$ = createNum($1);
    }
    | ID { 
        $$ = createVar($1);
        free($1);
    }
    | expr '+' expr { 
        $$ = createBinOp('+', $1, $3);
    }
    | expr '-' expr { 
        $$ = createBinOp('-', $1, $3);
    }
    | ID '[' expr ']' {
        /* Create an array access node */
        $$ = createArrayAccess($1, $3); /* NOTE: New AST function needed */
        free($1);
    }
    | ID '[' expr ']' '[' expr ']'
    { 
        $$ = createArray2DAccess($1, $3, $6); 
    }
    | ID '(' arg_list ')' { $$ = createFuncCall($1, $3); }
    | ID '(' ')' { $$ = createFuncCall($1, NULL); }
    ;

arg_list:
    expr { $$ = $1; }
    | arg_list ',' expr { $$ = createArgList($1, $3); }
    ;

/* PRINT STATEMENT - "print(expr);" */
print_stmt:
    PRINT '(' expr ')' ';' { 
        /* Create print node with expression to print */
        $$ = createPrint($3);  /* $3 is the expression inside parens */
    }
    ;

%%

/* ERROR HANDLING - Called by Bison when syntax error detected */
void yyerror(const char* s) {
    fprintf(stderr, "Syntax Error: %s\n", s);
}