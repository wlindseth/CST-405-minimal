#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symtab.h"

FILE* output;
int tempReg = 0;
int inFunction = 0;
int localVarCount = 0;

int getNextTemp() {
    int reg = tempReg++;
    if (tempReg > 7) tempReg = 0;
    return reg;
}

// Helper function to collect parameters from parameter list
int collectParameters(ASTNode* paramNode, ASTNode** params) {
    if (!paramNode) return 0;
    
    int count = 0;
    
    if (paramNode->type == NODE_PARAM) {
        // Single parameter
        params[count++] = paramNode;
    } else if (paramNode->type == NODE_PARAM_LIST) {
        // Recursive list - collect from left side first
        count += collectParameters(paramNode->data.list.item, params + count);
        // Then from right side
        count += collectParameters(paramNode->data.list.next, params + count);
    }
    
    return count;
}

void genExpr(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_NUM:
            fprintf(output, "    li $t%d, %d\n", getNextTemp(), node->data.num);
            break;
            
        case NODE_VAR: {
            Symbol* sym = lookupSymbol(node->data.name);
            if (!sym) {
                fprintf(stderr, "Error: Variable %s not declared\n", node->data.name);
                exit(1);
            }
            int offset = sym->offset;
            fprintf(output, "    lw $t%d, %d($fp)\n", getNextTemp(), offset);
            break;
        }
        
        case NODE_BINOP:
            genExpr(node->data.binop.left);
            int leftReg = tempReg - 1;
            genExpr(node->data.binop.right);
            int rightReg = tempReg - 1;
            
            if (node->data.binop.op == '+') {
                fprintf(output, "    add $t%d, $t%d, $t%d\n", leftReg, leftReg, rightReg);
            } else if (node->data.binop.op == '-') {
                fprintf(output, "    sub $t%d, $t%d, $t%d\n", leftReg, leftReg, rightReg);
            } else if (node->data.binop.op == '*') {
                fprintf(output, "    mul $t%d, $t%d, $t%d\n", leftReg, leftReg, rightReg);
            }
            tempReg = leftReg + 1;
            break;
        
        case NODE_ARRAY_ACCESS: {
            Symbol* sym = lookupSymbol(node->data.array_access.name);
            if (!sym) {
                fprintf(stderr, "Error: Array %s not declared\n", node->data.array_access.name);
                exit(1);
            }
            int baseOffset = sym->offset;
            
            genExpr(node->data.array_access.index);
            int indexReg = tempReg - 1;
            
            fprintf(output, "    sll $t%d, $t%d, 2\n", indexReg, indexReg);
            fprintf(output, "    addi $t%d, $fp, %d\n", getNextTemp(), baseOffset);
            int baseReg = tempReg - 1;
            fprintf(output, "    add $t%d, $t%d, $t%d\n", baseReg, baseReg, indexReg);
            fprintf(output, "    lw $t%d, 0($t%d)\n", indexReg, baseReg);
            tempReg = indexReg + 1;
            break;
        }

        case NODE_ARRAY_2D_ACCESS: {
            Symbol* sym = getSymbol(node->data.array_2d_access.name);
            if (!sym || !sym->is2DArray) {
                fprintf(stderr, "Error: 2D Array %s not declared\n", node->data.array_2d_access.name);
                exit(1);
            }
            
            int baseOffset = sym->offset;
            int cols = sym->cols;
            
            genExpr(node->data.array_2d_access.row);
            int rowReg = tempReg - 1;
            genExpr(node->data.array_2d_access.col);
            int colReg = tempReg - 1;
            
            fprintf(output, "    li $t%d, %d\n", getNextTemp(), cols);
            int colsReg = tempReg - 1;
            fprintf(output, "    mul $t%d, $t%d, $t%d\n", rowReg, rowReg, colsReg);
            fprintf(output, "    add $t%d, $t%d, $t%d\n", rowReg, rowReg, colReg);
            fprintf(output, "    sll $t%d, $t%d, 2\n", rowReg, rowReg);
            fprintf(output, "    addi $t%d, $fp, %d\n", colsReg, baseOffset);
            fprintf(output, "    add $t%d, $t%d, $t%d\n", colsReg, colsReg, rowReg);
            fprintf(output, "    lw $t%d, 0($t%d)\n", rowReg, colsReg);
            tempReg = rowReg + 1;
            break;
        }
        
        case NODE_FUNC_CALL: {
            // Save $ra if we're in a function (nested calls)
            if (inFunction) {
                fprintf(output, "    # Save $ra before nested call\n");
                fprintf(output, "    addi $sp, $sp, -4\n");
                fprintf(output, "    sw $ra, 0($sp)\n");
            }
            
            // Count arguments and collect them into array
            int argCount = 0;
            ASTNode* args[10];
            
            // Flatten the argument list
            ASTNode* argNode = node->data.func_call.args;
            while (argNode) {
                if (argNode->type == NODE_ARG_LIST) {
                    args[argCount++] = argNode->data.list.item;
                    argNode = argNode->data.list.next;
                } else {
                    args[argCount++] = argNode;
                    break;
                }
            }
            
            // Reverse the args array (they were built backwards)
            for (int i = 0; i < argCount / 2; i++) {
                ASTNode* temp = args[i];
                args[i] = args[argCount - 1 - i];
                args[argCount - 1 - i] = temp;
            }
            
            // Load arguments into $a0-$a3 (max 4 args for simplicity)
            for (int i = 0; i < argCount && i < 4; i++) {
                genExpr(args[i]);
                fprintf(output, "    move $a%d, $t%d\n", i, tempReg - 1);
            }
            
            // Call the function - add func_ prefix unless it's main
            if (strcmp(node->data.func_call.name, "main") == 0) {
                fprintf(output, "    jal %s\n", node->data.func_call.name);
            } else {
                fprintf(output, "    jal func_%s\n", node->data.func_call.name);
            }
            
            // Restore $ra if needed
            if (inFunction) {
                fprintf(output, "    # Restore $ra after nested call\n");
                fprintf(output, "    lw $ra, 0($sp)\n");
                fprintf(output, "    addi $sp, $sp, 4\n");
            }
            
            // Move return value to temp register
            fprintf(output, "    move $t%d, $v0\n", getNextTemp());
            break;
        }
            
        default:
            break;
    }
}

void genStmt(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_FUNC_LIST:
            genStmt(node->data.list.item);
            genStmt(node->data.list.next);
            break;
            
        case NODE_FUNC_DECL: {
            inFunction = 1;
            localVarCount = 0;
            
            // Generate function label - don't mangle main
            fprintf(output, "\n");
            if (strcmp(node->data.func_decl.name, "main") == 0) {
                fprintf(output, "# Function: %s\n", node->data.func_decl.name);
                fprintf(output, "%s:\n", node->data.func_decl.name);
            } else {
                fprintf(output, "# Function: %s\n", node->data.func_decl.name);
                fprintf(output, "func_%s:\n", node->data.func_decl.name);
            }
            
            // Function prologue
            fprintf(output, "    # Prologue\n");
            fprintf(output, "    addi $sp, $sp, -8\n");
            fprintf(output, "    sw $ra, 4($sp)\n");
            fprintf(output, "    sw $fp, 0($sp)\n");
            fprintf(output, "    move $fp, $sp\n");
            
            // Enter scope for this function
            enterScope();
            
            // Collect all parameters using helper function
            ASTNode* params[10];
            int paramCount = collectParameters(node->data.func_decl.params, params);
            
            // Add parameters to symbol table
            for (int i = 0; i < paramCount; i++) {
                fprintf(output, "    # Parameter %d: %s\n", i, params[i]->data.param.name);
                addParameter(params[i]->data.param.name, params[i]->data.param.type);
            }
            
            // Save argument registers to parameter locations
            for (int i = 0; i < paramCount && i < 4; i++) {
                fprintf(output, "    sw $a%d, %d($fp)\n", i, 8 + i * 4);
            }
            
            // Count local variables to allocate space
            int localCount = countLocalVars(node->data.func_decl.body);
            if (localCount > 0) {
                fprintf(output, "    # Allocate space for %d local variables\n", localCount);
                fprintf(output, "    addi $sp, $sp, %d\n", -(localCount * 4));
            }
            
            // Generate function body
            genStmt(node->data.func_decl.body);
            
            // Function epilogue (if no explicit return)
            fprintf(output, "    # Epilogue\n");
            if (localCount > 0) {
                fprintf(output, "    addi $sp, $sp, %d\n", localCount * 4);
            }
            fprintf(output, "    move $sp, $fp\n");
            fprintf(output, "    lw $fp, 0($sp)\n");
            fprintf(output, "    lw $ra, 4($sp)\n");
            fprintf(output, "    addi $sp, $sp, 8\n");
            fprintf(output, "    jr $ra\n");
            
            exitScope();
            inFunction = 0;
            break;
        }
        
        case NODE_DECL: {
            addVar(node->data.name);
            localVarCount++;
            fprintf(output, "    # Declared %s\n", node->data.name);
            break;
        }
        
        case NODE_ARRAY_DECL: {
            addArray(node->data.array_decl.name, node->data.array_decl.size);
            fprintf(output, "    # Declared array %s[%d]\n", 
                    node->data.array_decl.name, node->data.array_decl.size);
            break;
        }

        case NODE_ARRAY_2D_DECL: {
            addArray2D(node->data.array_2d_decl.name, 
                       node->data.array_2d_decl.rows, 
                       node->data.array_2d_decl.cols);
            fprintf(output, "    # Declared 2D array %s[%d][%d]\n", 
                    node->data.array_2d_decl.name, 
                    node->data.array_2d_decl.rows, 
                    node->data.array_2d_decl.cols);
            break;
        }
        
        case NODE_ASSIGN: {
            Symbol* sym = lookupSymbol(node->data.assign.var);
            if (!sym) {
                fprintf(stderr, "Error: Variable %s not declared\n", node->data.assign.var);
                exit(1);
            }
            int offset = sym->offset;
            genExpr(node->data.assign.value);
            fprintf(output, "    sw $t%d, %d($fp)\n", tempReg - 1, offset);
            tempReg = 0;
            break;
        }
        
        case NODE_ARRAY_ASSIGN: {
            Symbol* sym = lookupSymbol(node->data.array_assign.name);
            if (!sym) {
                fprintf(stderr, "Error: Array %s not declared\n", node->data.array_assign.name);
                exit(1);
            }
            int baseOffset = sym->offset;
            
            genExpr(node->data.array_assign.index);
            int indexReg = tempReg - 1;
            
            fprintf(output, "    sll $t%d, $t%d, 2\n", indexReg, indexReg);
            fprintf(output, "    addi $t%d, $fp, %d\n", getNextTemp(), baseOffset);
            int baseReg = tempReg - 1;
            fprintf(output, "    add $t%d, $t%d, $t%d\n", baseReg, baseReg, indexReg);
            
            genExpr(node->data.array_assign.value);
            int valueReg = tempReg - 1;
            
            fprintf(output, "    sw $t%d, 0($t%d)\n", valueReg, baseReg);
            tempReg = 0;
            break;
        }

        case NODE_ARRAY_2D_ASSIGN: {
            Symbol* sym = getSymbol(node->data.array_2d_assign.name);
            if (!sym || !sym->is2DArray) {
                fprintf(stderr, "Error: 2D Array %s not declared\n", node->data.array_2d_assign.name);
                exit(1);
            }
            
            int baseOffset = sym->offset;
            int cols = sym->cols;
            
            genExpr(node->data.array_2d_assign.row);
            int rowReg = tempReg - 1;
            genExpr(node->data.array_2d_assign.col);
            int colReg = tempReg - 1;
            
            fprintf(output, "    li $t%d, %d\n", getNextTemp(), cols);
            int colsReg = tempReg - 1;
            fprintf(output, "    mul $t%d, $t%d, $t%d\n", rowReg, rowReg, colsReg);
            fprintf(output, "    add $t%d, $t%d, $t%d\n", rowReg, rowReg, colReg);
            fprintf(output, "    sll $t%d, $t%d, 2\n", rowReg, rowReg);
            fprintf(output, "    addi $t%d, $fp, %d\n", colsReg, baseOffset);
            fprintf(output, "    add $t%d, $t%d, $t%d\n", colsReg, colsReg, rowReg);
            
            genExpr(node->data.array_2d_assign.value);
            int valueReg = tempReg - 1;
            
            fprintf(output, "    sw $t%d, 0($t%d)\n", valueReg, colsReg);
            tempReg = 0;
            break;
        }
        
        case NODE_PRINT:
            genExpr(node->data.expr);
            fprintf(output, "    # Print integer\n");
            fprintf(output, "    move $a0, $t%d\n", tempReg - 1);
            fprintf(output, "    li $v0, 1\n");
            fprintf(output, "    syscall\n");
            fprintf(output, "    # Print newline\n");
            fprintf(output, "    li $v0, 11\n");
            fprintf(output, "    li $a0, 10\n");
            fprintf(output, "    syscall\n");
            tempReg = 0;
            break;
        
        case NODE_RETURN: {
            if (node->data.return_expr) {
                genExpr(node->data.return_expr);
                fprintf(output, "    move $v0, $t%d\n", tempReg - 1);
            }
            
            // Count locals for proper deallocation
            fprintf(output, "    # Return statement\n");
            if (localVarCount > 0) {
                fprintf(output, "    addi $sp, $sp, %d\n", localVarCount * 4);
            }
            fprintf(output, "    move $sp, $fp\n");
            fprintf(output, "    lw $fp, 0($sp)\n");
            fprintf(output, "    lw $ra, 4($sp)\n");
            fprintf(output, "    addi $sp, $sp, 8\n");
            fprintf(output, "    jr $ra\n");
            tempReg = 0;
            break;
        }
            
        case NODE_STMT_LIST:
            genStmt(node->data.stmtlist.stmt);
            genStmt(node->data.stmtlist.next);
            break;
            
        default:
            break;
    }
}

// Helper function to count local variables in function body
int countLocalVars(ASTNode* node) {
    if (!node) return 0;
    
    int count = 0;
    switch(node->type) {
        case NODE_DECL:
            return 1;
        case NODE_ARRAY_DECL:
            return node->data.array_decl.size;
        case NODE_ARRAY_2D_DECL:
            return node->data.array_2d_decl.rows * node->data.array_2d_decl.cols;
        case NODE_STMT_LIST:
            count += countLocalVars(node->data.stmtlist.stmt);
            count += countLocalVars(node->data.stmtlist.next);
            return count;
        default:
            return 0;
    }
}

void generateMIPS(ASTNode* root, const char* filename) {
    output = fopen(filename, "w");
    if (!output) {
        fprintf(stderr, "Cannot open output file %s\n", filename);
        exit(1);
    }
    
    // Initialize symbol table
    initSymTab();
    
    // MIPS program header - proper SPIM format
    fprintf(output, ".data\n");
    fprintf(output, "\n");
    fprintf(output, ".text\n");
    fprintf(output, ".globl main\n");
    fprintf(output, "\n");
    
    // Generate code for all functions
    genStmt(root);
    
    // Add exit syscall at the end if main doesn't return properly
    fprintf(output, "\n# Exit program\n");
    fprintf(output, "_exit:\n");
    fprintf(output, "    li $v0, 10\n");
    fprintf(output, "    syscall\n");
    
    fclose(output);
}