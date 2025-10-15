#ifndef SYMTAB_H
#define SYMTAB_H

/* SYMBOL TABLE (Hierarchical)
 * This symbol table implements a stack of scopes to handle nested
 * variable and function declarations, crucial for a language with functions.
 */

#define MAX_SYMBOLS_PER_SCOPE 100

// Enhanced symbol entry
typedef struct {
    char* name;
    char* type;      // Variable type ("int") or function return type
    int offset;      // Stack offset (for local variables and parameters)
    int isFunction;  // 1 if this is a function, 0 if variable
    
    // -- Function-specific data --
    int paramCount;  // Number of parameters
    char** paramTypes; // Array of parameter types (e.g., {"int", "int"})
} Symbol;

// Scope structure
typedef struct Scope {
    Symbol symbols[MAX_SYMBOLS_PER_SCOPE];
    int count;
    int nextOffset;
    struct Scope* parent;  // Link to the enclosing (parent) scope
} Scope;

// Symbol table with a scope stack
typedef struct {
    Scope* currentScope;   // Top of the scope stack (the current scope)
    Scope* globalScope;    // Always points to the global scope
} SymbolTable;


/* --- SYMBOL TABLE OPERATIONS --- */

// Initialization
void initSymTab();

// Scope Management
void enterScope();              // Push a new scope onto the stack
void exitScope();               // Pop the current scope from the stack

// Symbol Lookup
Symbol* lookupSymbol(char* name);  // Search from current scope outwards to global
int isInCurrentScope(char* name);  // Check for a symbol ONLY in the current scope

// Symbol Addition
int addVar(char* name, char* type);
int addParameter(char* name, char* type);
int addFunction(char* name, char* returnType, char** paramTypes, int paramCount);

// For debugging
void printSymTab();

#endif