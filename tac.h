#ifndef TAC_H
#define TAC_H

#include "ast.h"

/* TAC INSTRUCTION TYPES */
typedef enum {
    TAC_ADD,
    TAC_ASSIGN,
    TAC_SUB,
    TAC_PRINT,
    TAC_DECL,
    TAC_DECL_ARRAY,
    TAC_STORE,
    TAC_LOAD,
    TAC_DECL_ARRAY_2D,
    TAC_STORE_2D,
    TAC_LOAD_2D,
    TAC_MUL,
    /* NEW: Function-related operations */
    TAC_LABEL,        // Function entry label
    TAC_FUNC_BEGIN,   // Mark function start
    TAC_FUNC_END,     // Mark function end
    TAC_PARAM,        // Push parameter for call
    TAC_CALL,         // Function call
    TAC_RETURN        // Return from function
} TACOp;

/* TAC INSTRUCTION STRUCTURE */
typedef struct TACInstr {
    TACOp op;
    char* arg1;
    char* arg2;
    char* arg3;
    char* result;
    int paramCount;       // For CALL: number of parameters
    struct TACInstr* next;
} TACInstr;

/* TAC LIST MANAGEMENT */
typedef struct {
    TACInstr* head;
    TACInstr* tail;
    int tempCount;
} TACList;

/* TAC GENERATION FUNCTIONS */
void initTAC();
char* newTemp();
TACInstr* createTAC(TACOp op, char* arg1, char* arg2, char* result);
TACInstr* createTAC2D(TACOp op, char* arg1, char* arg2, char* arg3, char* result);
void appendTAC(TACInstr* instr);
void generateTAC(ASTNode* node);
char* generateTACExpr(ASTNode* node);

/* TAC OPTIMIZATION AND OUTPUT */
void printTAC();
void optimizeTAC();
void printOptimizedTAC();

#endif