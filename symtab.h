#ifndef SYMTAB_H
#define SYMTAB_H

#define MAX_VARS 100
#define MAX_PARAMS 10

/* Symbol entry */
typedef struct {
    char* name;
    int offset;
    int isArray;
    int arraySize;
    int is2DArray;
    int rows;
    int cols;
    int isFunction;
    int isParameter;
    char* returnType;
    int paramCount;
} Symbol;

/* Forward declaration */
struct Scope;

/* Scope structure for nested scopes */
typedef struct Scope {
    Symbol vars[MAX_VARS];
    int count;
    int nextOffset;
    int paramOffset;
    struct Scope* parent;
} Scope;

/* Symbol table with scope management */
typedef struct {
    Scope* currentScope;
    Scope* globalScope;
} SymbolTable;

/* Global symbol table instance */
extern SymbolTable symtab;

/* Basic operations */
void initSymTab();
int addVar(char* name);
int addArray(char* name, int size);
int addArray2D(char* name, int rows, int cols);
int getVarOffset(char* name);
int isVarDeclared(char* name);
Symbol* getSymbol(char* name);

/* Function and scope operations */
void enterScope();
void exitScope();
int addFunction(char* name, char* returnType, int paramCount);
int addParameter(char* name, char* type);
Symbol* lookupSymbol(char* name);
int isInCurrentScope(char* name);

#endif