/* SYMBOL TABLE IMPLEMENTATION (Hierarchical)
 * Manages a stack of scopes to support function-level scoping.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* Global symbol table instance */
SymbolTable symtab;

/* Initialize the symbol table with a global scope */
void initSymTab() {
    // Create the global scope
    symtab.globalScope = (Scope*) malloc(sizeof(Scope));
    symtab.globalScope->count = 0;
    symtab.globalScope->nextOffset = 0;
    symtab.globalScope->parent = NULL; // Global scope has no parent

    // The current scope starts as the global scope
    symtab.currentScope = symtab.globalScope;
}

/* Enter a new scope (e.g., when entering a function body) */
void enterScope() {
    Scope* newScope = (Scope*) malloc(sizeof(Scope));
    newScope->count = 0;
    newScope->nextOffset = 0; // Each function scope has its own offset count
    newScope->parent = symtab.currentScope; // Link to the parent scope
    symtab.currentScope = newScope; // The new scope is now the current one
}

/* Exit the current scope (e.g., when leaving a function body) */
void exitScope() {
    // Cannot exit the global scope
    if (symtab.currentScope == symtab.globalScope) {
        return;
    }

    Scope* parentScope = symtab.currentScope->parent;
    
    // Free memory for names/types in the exiting scope
    for (int i = 0; i < symtab.currentScope->count; i++) {
        free(symtab.currentScope->symbols[i].name);
        free(symtab.currentScope->symbols[i].type);
        // Note: paramTypes are not freed here as they are managed by the parser
    }
    
    free(symtab.currentScope); // Free the scope structure itself
    symtab.currentScope = parentScope; // Move up to the parent scope
}

/* Look up a symbol by searching from the current scope outwards */
Symbol* lookupSymbol(char* name) {
    Scope* scope = symtab.currentScope;

    // Search from current scope up to global
    while (scope != NULL) {
        for (int i = 0; i < scope->count; i++) {
            if (strcmp(scope->symbols[i].name, name) == 0) {
                return &scope->symbols[i];  // Found it
            }
        }
        scope = scope->parent;  // Try parent scope
    }
    return NULL;  // Not found in any scope
}

/* Check if a symbol exists ONLY in the current scope (for detecting re-declarations) */
int isInCurrentScope(char* name) {
    for (int i = 0; i < symtab.currentScope->count; i++) {
        if (strcmp(symtab.currentScope->symbols[i].name, name) == 0) {
            return 1; // Found
        }
    }
    return 0; // Not found
}

/* Add a local variable to the current scope */
int addVar(char* name, char* type) {
    if (isInCurrentScope(name)) {
        fprintf(stderr, "Semantic Error: Redeclaration of variable '%s'\n", name);
        return -1; // Error: already declared in this scope
    }
    if (symtab.currentScope->count >= MAX_SYMBOLS_PER_SCOPE) {
        fprintf(stderr, "Error: Symbol table full in current scope.\n");
        return -1;
    }

    Symbol* newSymbol = &symtab.currentScope->symbols[symtab.currentScope->count];
    newSymbol->name = strdup(name);
    newSymbol->type = strdup(type);
    newSymbol->offset = symtab.currentScope->nextOffset;
    newSymbol->isFunction = 0;
    newSymbol->paramCount = 0;
    newSymbol->paramTypes = NULL;
    
    symtab.currentScope->nextOffset += 4; // Assuming 4-byte integers
    symtab.currentScope->count++;
    
    return newSymbol->offset;
}

/* Add a function parameter to the current scope */
int addParameter(char* name, char* type) {
    // Adding a parameter is the same as adding a local variable
    return addVar(name, type);
}

/* Add a function declaration to the current scope (should be global) */
int addFunction(char* name, char* returnType, char** paramTypes, int paramCount) {
    if (isInCurrentScope(name)) {
        fprintf(stderr, "Semantic Error: Redeclaration of function '%s'\n", name);
        return -1;
    }
    if (symtab.currentScope->count >= MAX_SYMBOLS_PER_SCOPE) {
        fprintf(stderr, "Error: Symbol table full in current scope.\n");
        return -1;
    }

    Symbol* newSymbol = &symtab.currentScope->symbols[symtab.currentScope->count];
    newSymbol->name = strdup(name);
    newSymbol->type = strdup(returnType); // 'type' holds the return type
    newSymbol->offset = -1; // Functions don't have a stack offset
    newSymbol->isFunction = 1;
    newSymbol->paramCount = paramCount;
    newSymbol->paramTypes = paramTypes;

    symtab.currentScope->count++;
    return 1; // Success
}

/* Print the entire symbol table stack for debugging */
void printSymTab() {
    printf("\n--- SYMBOL TABLE (SCOPED) ---\n");
    Scope* scope = symtab.currentScope;
    int level = 0;
    while(scope != NULL) {
        if (scope == symtab.globalScope) {
            printf("--- GLOBAL SCOPE ---\n");
        } else {
            printf("--- LOCAL SCOPE (Level %d) ---\n", level);
        }
        
        if (scope->count == 0) {
            printf("(empty)\n");
        } else {
             for (int i = 0; i < scope->count; i++) {
                Symbol* s = &scope->symbols[i];
                if (s->isFunction) {
                    printf("  FUNC: %s, returns %s, params: %d\n", s->name, s->type, s->paramCount);
                } else {
                    printf("  VAR: %s, type %s, offset %d\n", s->name, s->type, s->offset);
                }
            }
        }
        scope = scope->parent;
        level++;
    }
    printf("---------------------------\n\n");
}