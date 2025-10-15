#ifndef TAC_H
#define TAC_H

#include "ast.h"

/* THREE-ADDRESS CODE (TAC)
 * Intermediate representation between AST and machine code
 * Each instruction has at most 3 operands (result = arg1 op arg2)
 * Makes optimization and code generation easier
 */

/* TAC INSTRUCTION TYPES */
typedef enum {
    TAC_ADD,
    TAC_ASSIGN,
    TAC_SUB,
    TAC_PRINT,
    TAC_DECL,
    /* --- ADD THESE FOR ARRAY SUPPORT --- */
    TAC_DECL_ARRAY, /* Declare array: DECL_ARRAY name, size */
    TAC_STORE,      /* Store into array: STORE name, index, value (e.g., arr[i] = x) */
    TAC_LOAD,        /* Load from array: LOAD result, name, index (e.g., x = arr[i]) */
    TAC_DECL_ARRAY_2D,  /* Declare 2D array: DECL_ARRAY_2D name, rows, cols */
    TAC_STORE_2D,       /* Store into 2D array: STORE_2D name, row, col, value */
    TAC_LOAD_2D,        /* Load from 2D array: LOAD_2D result, name, row, col */
    TAC_MUL,             /* Multiply for address calculation */
    /* FOR FUNCTIONS */
    TAC_LABEL,       // Function entry point: LABEL func_name
    TAC_PARAM,       // Pass parameter: PARAM arg
    TAC_CALL,        // Function call: result = CALL func_name, num_params
    TAC_RETURN,      // Return value: RETURN value
    TAC_FUNC_BEGIN,  // Mark function start: FUNC_BEGIN name
    TAC_FUNC_END     // Mark function end: FUNC_END name
} TACOp;

/* TAC INSTRUCTION STRUCTURE */
typedef struct TACInstr {
    TACOp op;               /* Operation type */
    char* arg1;             /* First operand (if needed) */
    char* arg2;             /* Second operand (for binary ops) */
    char* arg3;             /* Third operand (for 2D arrays) */
    char* result;           /* Result/destination */
    int paramCount;  // For CALL instruction: number of params
    struct TACInstr* next;  /* Linked list pointer */
} TACInstr;

/* TAC LIST MANAGEMENT */
typedef struct {
    TACInstr* head;    /* First instruction */
    TACInstr* tail;    /* Last instruction (for efficient append) */
    int tempCount;     /* Counter for temporary variables (t0, t1, ...) */
} TACList;

/* TAC GENERATION FUNCTIONS */
void initTAC();                                                    /* Initialize TAC lists */
char* newTemp();                                                   /* Generate new temp variable */
TACInstr* createTAC(TACOp op, char* arg1, char* arg2, char* result); /* Create TAC instruction */
TACInstr* createTAC2D(TACOp op, char* arg1, char* arg2, char* arg3, char* result);
void appendTAC(TACInstr* instr);                                  /* Add instruction to list */
void generateTAC(ASTNode* node);                                  /* Convert AST to TAC */
char* generateTACExpr(ASTNode* node);                             /* Generate TAC for expression */

/* TAC OPTIMIZATION AND OUTPUT */
void printTAC();                                                   /* Display unoptimized TAC */
void optimizeTAC();                                                /* Apply optimizations */
void printOptimizedTAC();                                          /* Display optimized TAC */

#endif