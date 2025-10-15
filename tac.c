#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tac.h"

TACList tacList;
TACList optimizedList;

void initTAC() {
    tacList.head = NULL;
    tacList.tail = NULL;
    tacList.tempCount = 0;
    optimizedList.head = NULL;
    optimizedList.tail = NULL;
}

char* newTemp() {
    char* temp = malloc(10);
    sprintf(temp, "t%d", tacList.tempCount++);
    return temp;
}

TACInstr* createTAC(TACOp op, char* arg1, char* arg2, char* result) {
    TACInstr* instr = malloc(sizeof(TACInstr));
    instr->op = op;
    instr->arg1 = arg1 ? strdup(arg1) : NULL;
    instr->arg2 = arg2 ? strdup(arg2) : NULL;
    instr->arg3 = NULL;
    instr->result = result ? strdup(result) : NULL;
    instr->next = NULL;
    return instr;
}

TACInstr* createTAC2D(TACOp op, char* arg1, char* arg2, char* arg3, char* result) {
    TACInstr* instr = malloc(sizeof(TACInstr));
    instr->op = op;
    instr->arg1 = arg1 ? strdup(arg1) : NULL;
    instr->arg2 = arg2 ? strdup(arg2) : NULL;
    instr->arg3 = arg3 ? strdup(arg3) : NULL;
    instr->result = result ? strdup(result) : NULL;
    instr->next = NULL;
    return instr;
}

void appendTAC(TACInstr* instr) {
    if (!tacList.head) {
        tacList.head = tacList.tail = instr;
    } else {
        tacList.tail->next = instr;
        tacList.tail = instr;
    }
}

void appendOptimizedTAC(TACInstr* instr) {
    if (!optimizedList.head) {
        optimizedList.head = optimizedList.tail = instr;
    } else {
        optimizedList.tail->next = instr;
        optimizedList.tail = instr;
    }
}

char* generateTACExpr(ASTNode* node) {
    if (!node) return NULL;
    
    switch(node->type) {
        case NODE_NUM: {
            char* temp = malloc(20);
            sprintf(temp, "%d", node->data.num);
            return temp;
        }
        
        case NODE_VAR:
            return strdup(node->data.name);
        
        case NODE_BINOP: {
            char* left = generateTACExpr(node->data.binop.left);
            char* right = generateTACExpr(node->data.binop.right);
            char* temp = newTemp();
            
            if (node->data.binop.op == '+') {
                appendTAC(createTAC(TAC_ADD, left, right, temp));
            }
            else if (node->data.binop.op == '-') {
                appendTAC(createTAC(TAC_SUB, left, right, temp));
            }
            else if (node->data.binop.op == '*') {
                appendTAC(createTAC(TAC_MUL, left, right, temp));
            }
            
            return temp;
        }
        
        case NODE_ARRAY_ACCESS: {
            char* index = generateTACExpr(node->data.array_access.index);
            char* temp = newTemp();
            appendTAC(createTAC(TAC_LOAD, node->data.array_access.name, index, temp));
            return temp;
        }

        case NODE_ARRAY_2D_ACCESS: {
            char* row = generateTACExpr(node->data.array_2d_access.row);
            char* col = generateTACExpr(node->data.array_2d_access.col);
            char* temp = newTemp();
            appendTAC(createTAC2D(TAC_LOAD_2D, node->data.array_2d_access.name, row, col, temp));
            return temp;
        }
        
        default:
            return NULL;
    }
}

void generateTAC(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_DECL:
            appendTAC(createTAC(TAC_DECL, NULL, NULL, node->data.name));
            break;

        case NODE_ARRAY_DECL: {
            char sizeStr[20];
            sprintf(sizeStr, "%d", node->data.array_decl.size);
            appendTAC(createTAC(TAC_DECL_ARRAY, strdup(sizeStr), NULL, node->data.array_decl.name));
            break;
        }
        
        case NODE_ARRAY_2D_DECL: {
            char rowStr[20], colStr[20];
            sprintf(rowStr, "%d", node->data.array_2d_decl.rows);
            sprintf(colStr, "%d", node->data.array_2d_decl.cols);
            appendTAC(createTAC(TAC_DECL_ARRAY_2D, strdup(rowStr), strdup(colStr), node->data.array_2d_decl.name));
            break;
        }

        case NODE_ASSIGN: {
            char* expr = generateTACExpr(node->data.assign.value);
            appendTAC(createTAC(TAC_ASSIGN, expr, NULL, node->data.assign.var));
            break;
        }

        case NODE_ARRAY_ASSIGN: {
            char* index = generateTACExpr(node->data.array_assign.index);
            char* value = generateTACExpr(node->data.array_assign.value);
            appendTAC(createTAC(TAC_STORE, index, value, node->data.array_assign.name));
            break;
        }

        case NODE_ARRAY_2D_ASSIGN: {
            char* row = generateTACExpr(node->data.array_2d_assign.row);
            char* col = generateTACExpr(node->data.array_2d_assign.col);
            char* value = generateTACExpr(node->data.array_2d_assign.value);
            appendTAC(createTAC2D(TAC_STORE_2D, row, col, value, node->data.array_2d_assign.name));
            break;
        }
        
        case NODE_PRINT: {
            char* expr = generateTACExpr(node->data.expr);
            appendTAC(createTAC(TAC_PRINT, expr, NULL, NULL));
            break;
        }
        
        case NODE_STMT_LIST:
            generateTAC(node->data.stmtlist.stmt);
            generateTAC(node->data.stmtlist.next);
            break;

        case NODE_FUNC_DECL:
            appendTAC(createTAC(TAC_FUNC_BEGIN, NULL, NULL,
                                node->data.func_decl.name));
            appendTAC(createTAC(TAC_LABEL, NULL, NULL,
                                node->data.func_decl.name));
            // Generate TAC for function body
            generateTAC(node->data.func_decl.body);
            appendTAC(createTAC(TAC_FUNC_END, NULL, NULL,
                                node->data.func_decl.name));
            break;

        case NODE_FUNC_CALL: {
            // Generate TAC for each argument
            ASTNode* arg = node->data.func_call.args;
            int paramCount = 0;
            while (arg) {
                char* argVal = generateTACExpr(arg->data.list.item);
                appendTAC(createTAC(TAC_PARAM, argVal, NULL, NULL));
                paramCount++;
                arg = arg->data.list.next;
            }
            // Generate the call
            char* temp = newTemp();
            TACInstr* call = createTAC(TAC_CALL, node->data.func_call.name,
                                    NULL, temp);
            call->paramCount = paramCount;
            appendTAC(call);
            return temp;
        }

        case NODE_RETURN: {
            if (node->data.return_expr) {
                char* retVal = generateTACExpr(node->data.return_expr);
                appendTAC(createTAC(TAC_RETURN, retVal, NULL, NULL));
            } else {
                appendTAC(createTAC(TAC_RETURN, NULL, NULL, NULL));
            }
            break;
        }
            
        default:
            break;
    }
}

void printTAC() {
    printf("Unoptimized TAC Instructions:\n");
    printf("─────────────────────────────\n");
    TACInstr* curr = tacList.head;
    int lineNum = 1;
    while (curr) {
        printf("%2d: ", lineNum++);
        switch(curr->op) {
            case TAC_DECL:
                printf("DECL %s", curr->result);
                printf("           // Declare variable '%s'\n", curr->result);
                break;
            case TAC_ADD:
                printf("%s = %s + %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Add: store result in %s\n", curr->result);
                break;
            case TAC_SUB:
                printf("%s = %s - %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Subtract: store result in %s\n", curr->result);
                break;
            case TAC_MUL:
                printf("%s = %s * %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_ASSIGN:
                printf("%s = %s", curr->result, curr->arg1);
                printf("           // Assign value to %s\n", curr->result);
                break;
            case TAC_PRINT:
                printf("PRINT %s", curr->arg1);
                printf("           // Output value of %s\n", curr->arg1);
                break;
            case TAC_DECL_ARRAY:
                printf("DECL_ARRAY %s[%s]", curr->result, curr->arg1);
                printf("   // Declare array '%s' of size %s\n", curr->result, curr->arg1);
                break;
            case TAC_STORE:
                printf("%s[%s] = %s", curr->result, curr->arg1, curr->arg2);
                printf("       // Store value in array '%s'\n", curr->result);
                break;
            case TAC_LOAD:
                printf("%s = %s[%s]", curr->result, curr->arg1, curr->arg2);
                printf("       // Load value from array '%s'\n", curr->arg1);
                break;
            case TAC_DECL_ARRAY_2D:
                printf("DECL_ARRAY_2D %s[%s][%s]\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_STORE_2D:
                printf("%s[%s][%s] = %s\n", curr->result, curr->arg1, curr->arg2, curr->arg3);
                break;
            case TAC_LOAD_2D:
                printf("%s = %s[%s][%s]\n", curr->result, curr->arg1, curr->arg2, curr->arg3);
                break;
            default:
                break;
        }
        curr = curr->next;
    }
}

// Helper function to check if a string is a constant
int isConstant(char* str) {
    if (!str || str[0] == '\0') return 0;
    int i = 0;
    if (str[0] == '-') i = 1;  // Handle negative numbers
    for (; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

void optimizeTAC() {
    TACInstr* curr = tacList.head;
    
    // Copy propagation table - only track constants
    typedef struct {
        char* var;
        char* value;
    } VarValue;
    
    VarValue values[100];
    int valueCount = 0;
    
    while (curr) {
        TACInstr* newInstr = NULL;
        
        switch(curr->op) {
            case TAC_DECL:
                newInstr = createTAC(TAC_DECL, NULL, NULL, curr->result);
                break;
                
            case TAC_ADD: {
                char* left = curr->arg1;
                char* right = curr->arg2;
                
                // Only substitute if we have a constant value
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, left) == 0 && isConstant(values[i].value)) {
                        left = values[i].value;
                        break;
                    }
                }
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, right) == 0 && isConstant(values[i].value)) {
                        right = values[i].value;
                        break;
                    }
                }
                
                // Only fold if BOTH operands are constants
                if (isConstant(left) && isConstant(right)) {
                    int result = atoi(left) + atoi(right);
                    char* resultStr = malloc(20);
                    sprintf(resultStr, "%d", result);
                    
                    values[valueCount].var = strdup(curr->result);
                    values[valueCount].value = resultStr;
                    valueCount++;
                    
                    newInstr = createTAC(TAC_ASSIGN, resultStr, NULL, curr->result);
                } else {
                    newInstr = createTAC(TAC_ADD, left, right, curr->result);
                }
                break;
            }

            case TAC_SUB: {
                char* left = curr->arg1;
                char* right = curr->arg2;
                
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, left) == 0 && isConstant(values[i].value)) {
                        left = values[i].value;
                        break;
                    }
                }
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, right) == 0 && isConstant(values[i].value)) {
                        right = values[i].value;
                        break;
                    }
                }
                
                if (isConstant(left) && isConstant(right)) {
                    int result = atoi(left) - atoi(right);
                    char* resultStr = malloc(20);
                    sprintf(resultStr, "%d", result);
                    
                    values[valueCount].var = strdup(curr->result);
                    values[valueCount].value = resultStr;
                    valueCount++;
                    
                    newInstr = createTAC(TAC_ASSIGN, resultStr, NULL, curr->result);
                } else {
                    newInstr = createTAC(TAC_SUB, left, right, curr->result);
                }
                break;
            }

            case TAC_MUL: {
                char* left = curr->arg1;
                char* right = curr->arg2;
                
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, left) == 0 && isConstant(values[i].value)) {
                        left = values[i].value;
                        break;
                    }
                }
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, right) == 0 && isConstant(values[i].value)) {
                        right = values[i].value;
                        break;
                    }
                }
                
                if (isConstant(left) && isConstant(right)) {
                    int result = atoi(left) * atoi(right);
                    char* resultStr = malloc(20);
                    sprintf(resultStr, "%d", result);
                    
                    values[valueCount].var = strdup(curr->result);
                    values[valueCount].value = resultStr;
                    valueCount++;
                    
                    newInstr = createTAC(TAC_ASSIGN, resultStr, NULL, curr->result);
                } else {
                    newInstr = createTAC(TAC_MUL, left, right, curr->result);
                }
                break;
            }
            
            case TAC_ASSIGN: {
                char* value = curr->arg1;
    
                // Remove any existing entry for this variable (it's being reassigned)
                for (int i = 0; i < valueCount; i++) {
                    if (strcmp(values[i].var, curr->result) == 0) {
                        // Shift all entries after this one down
                        for (int j = i; j < valueCount - 1; j++) {
                            values[j] = values[j + 1];
                        }
                        valueCount--;
                        break;
                    }
                }
    
                // Only track if the new value is a constant
                if (isConstant(value)) {
                    values[valueCount].var = strdup(curr->result);
                    values[valueCount].value = strdup(value);
                    valueCount++;
                }
    
                newInstr = createTAC(TAC_ASSIGN, value, NULL, curr->result);
                 break;
            }
            
            case TAC_PRINT: {
                char* value = curr->arg1;
                
                // Only substitute constants
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, value) == 0 && isConstant(values[i].value)) {
                        value = values[i].value;
                        break;
                    }
                }
                
                newInstr = createTAC(TAC_PRINT, value, NULL, NULL);
                break;
            }

            case TAC_DECL_ARRAY:
                newInstr = createTAC(TAC_DECL_ARRAY, curr->arg1, NULL, curr->result);
                break;
            
            case TAC_DECL_ARRAY_2D:
                newInstr = createTAC(TAC_DECL_ARRAY_2D, curr->arg1, curr->arg2, curr->result);
                break;

            case TAC_LOAD:
                newInstr = createTAC(TAC_LOAD, curr->arg1, curr->arg2, curr->result);
                break;
            
            case TAC_LOAD_2D:
                newInstr = createTAC2D(TAC_LOAD_2D, curr->arg1, curr->arg2, curr->arg3, curr->result);
                break;
            
            case TAC_STORE:
                newInstr = createTAC(TAC_STORE, curr->arg1, curr->arg2, curr->result);
                break;
            
            case TAC_STORE_2D:
                newInstr = createTAC2D(TAC_STORE_2D, curr->arg1, curr->arg2, curr->arg3, curr->result);
                break;
        }
        
        if (newInstr) {
            appendOptimizedTAC(newInstr);
        }
        
        curr = curr->next;
    }
}

void printOptimizedTAC() {
    printf("\nOptimized TAC Instructions:\n");
    printf("───────────────────────────\n");
    TACInstr* curr = optimizedList.head;
    int lineNum = 1;
    while (curr) {
        printf("%2d: ", lineNum++);
        switch(curr->op) {
            case TAC_DECL:
                printf("DECL %s\n", curr->result);
                break;
            case TAC_ADD:
                printf("%s = %s + %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_SUB:
                printf("%s = %s - %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_MUL:
                printf("%s = %s * %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_ASSIGN:
                printf("%s = %s\n", curr->result, curr->arg1);
                break;
            case TAC_PRINT:
                printf("PRINT %s\n", curr->arg1);
                break;
            case TAC_DECL_ARRAY:
                printf("DECL_ARRAY %s[%s]\n", curr->result, curr->arg1);
                break;
            case TAC_STORE:
                printf("%s[%s] = %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_LOAD:
                printf("%s = %s[%s]\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_DECL_ARRAY_2D:
                printf("DECL_ARRAY_2D %s[%s][%s]\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_STORE_2D:
                printf("%s[%s][%s] = %s\n", curr->result, curr->arg1, curr->arg2, curr->arg3);
                break;
            case TAC_LOAD_2D:  
                printf("%s = %s[%s][%s]\n", curr->result, curr->arg1, curr->arg2, curr->arg3);
                break;
            default:
                break;
        }
        curr = curr->next;
    }
}