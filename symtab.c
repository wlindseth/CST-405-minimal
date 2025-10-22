/* SYMBOL TABLE IMPLEMENTATION
 * Manages variable declarations and lookups
 * Essential for semantic analysis (checking if variables are declared)
 * Provides memory layout information for code generation
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

SymbolTable symtab;

void initSymTab() {
    // Create global scope
    symtab.globalScope = malloc(sizeof(Scope));
    symtab.globalScope->count = 0;
    symtab.globalScope->nextOffset = 0;
    symtab.globalScope->paramOffset = 8;
    symtab.globalScope->parent = NULL;
    symtab.currentScope = symtab.globalScope;
}

void enterScope() {
    Scope* newScope = malloc(sizeof(Scope));
    newScope->count = 0;
    newScope->nextOffset = -4;  // Local variables have negative offsets
    newScope->paramOffset = 8;   // Parameters have positive offsets
    newScope->parent = symtab.currentScope;
    symtab.currentScope = newScope;
}

void exitScope() {
    if (symtab.currentScope != symtab.globalScope) {
        Scope* oldScope = symtab.currentScope;
        symtab.currentScope = symtab.currentScope->parent;
        free(oldScope);
    }
}

Symbol* lookupSymbol(char* name) {
    Scope* scope = symtab.currentScope;
    
    while (scope != NULL) {
        for (int i = 0; i < scope->count; i++) {
            if (strcmp(scope->vars[i].name, name) == 0) {
                return &scope->vars[i];
            }
        }
        scope = scope->parent;
    }
    return NULL;
}

int isInCurrentScope(char* name) {
    for (int i = 0; i < symtab.currentScope->count; i++) {
        if (strcmp(symtab.currentScope->vars[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

int addVar(char* name) {
    if (isInCurrentScope(name)) {
        return -1;
    }
    
    Symbol* sym = &symtab.currentScope->vars[symtab.currentScope->count];
    sym->name = strdup(name);
    sym->isFunction = 0;
    sym->isParameter = 0;
    sym->isArray = 0;
    sym->is2DArray = 0;
    sym->offset = symtab.currentScope->nextOffset;
    
    symtab.currentScope->nextOffset -= 4;  // Locals grow downward
    symtab.currentScope->count++;
    
    return sym->offset;
}

int addFunction(char* name, char* returnType, int paramCount) {
    if (isInCurrentScope(name)) {
        return -1;
    }
    
    Symbol* sym = &symtab.currentScope->vars[symtab.currentScope->count];
    sym->name = strdup(name);
    sym->returnType = strdup(returnType);
    sym->isFunction = 1;
    sym->isParameter = 0;
    sym->paramCount = paramCount;
    sym->offset = 0;
    sym->isArray = 0;
    sym->is2DArray = 0;
    
    symtab.currentScope->count++;
    return 0;
}

int addParameter(char* name, char* type) {
    if (isInCurrentScope(name)) {
        return -1;
    }
    
    Symbol* sym = &symtab.currentScope->vars[symtab.currentScope->count];
    sym->name = strdup(name);
    sym->returnType = strdup(type);
    sym->isParameter = 1;
    sym->isFunction = 0;
    sym->isArray = 0;
    sym->is2DArray = 0;
    sym->offset = symtab.currentScope->paramOffset;
    
    symtab.currentScope->paramOffset += 4;
    symtab.currentScope->count++;
    
    return sym->offset;
}

int getVarOffset(char* name) {
    Symbol* sym = lookupSymbol(name);
    return sym ? sym->offset : -9999;
}

int isVarDeclared(char* name) {
    return lookupSymbol(name) != NULL;
}

Symbol* getSymbol(char* name) {
    return lookupSymbol(name);
}

int addArray(char* name, int size) {
    if (isInCurrentScope(name)) {
        return -1;
    }
    
    Symbol* sym = &symtab.currentScope->vars[symtab.currentScope->count];
    sym->name = strdup(name);
    sym->isFunction = 0;
    sym->isParameter = 0;
    sym->isArray = 1;
    sym->is2DArray = 0;
    sym->arraySize = size;
    sym->offset = symtab.currentScope->nextOffset;
    
    symtab.currentScope->nextOffset -= size * 4;
    symtab.currentScope->count++;
    
    return sym->offset;
}

int addArray2D(char* name, int rows, int cols) {
    if (isInCurrentScope(name)) {
        return -1;
    }
    
    Symbol* sym = &symtab.currentScope->vars[symtab.currentScope->count];
    sym->name = strdup(name);
    sym->isFunction = 0;
    sym->isParameter = 0;
    sym->isArray = 1;
    sym->is2DArray = 1;
    sym->rows = rows;
    sym->cols = cols;
    sym->arraySize = rows * cols;
    sym->offset = symtab.currentScope->nextOffset;
    
    symtab.currentScope->nextOffset -= rows * cols * 4;
    symtab.currentScope->count++;
    
    return sym->offset;
}

void printSymTab() {
    printf("\n=== SYMBOL TABLE STATE ===\n");
    Scope* scope = symtab.currentScope;
    int level = 0;
    
    while (scope != NULL) {
        printf("Scope Level %d: Count=%d, NextOffset=%d\n", level, scope->count, scope->nextOffset);
        if (scope->count == 0) {
            printf("  (empty)\n");
        } else {
            printf("  Variables:\n");
            for (int i = 0; i < scope->count; i++) {
                if (scope->vars[i].isFunction) {
                    printf("    [%d] FUNCTION %s -> returns %s, %d params\n", 
                           i, scope->vars[i].name, scope->vars[i].returnType, scope->vars[i].paramCount);
                } else if (scope->vars[i].is2DArray) {
                    printf("    [%d] %s[%d][%d] -> offset %d\n", 
                           i, scope->vars[i].name, scope->vars[i].rows, scope->vars[i].cols, scope->vars[i].offset);
                } else if (scope->vars[i].isArray) {
                    printf("    [%d] %s[%d] -> offset %d\n", 
                           i, scope->vars[i].name, scope->vars[i].arraySize, scope->vars[i].offset);
                } else {
                    printf("    [%d] %s -> offset %d%s\n", 
                           i, scope->vars[i].name, scope->vars[i].offset,
                           scope->vars[i].isParameter ? " (parameter)" : "");
                }
            }
        }
        scope = scope->parent;
        level++;
    }
    printf("==========================\n\n");
}