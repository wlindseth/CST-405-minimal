#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "symtab.h"

FILE* output;
int tempReg = 0;

int getNextTemp() {
    int reg = tempReg++;
    if (tempReg > 7) tempReg = 0;  // Reuse $t0-$t7
    return reg;
}

void genExpr(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_NUM:
            fprintf(output, "    li $t%d, %d\n", getNextTemp(), node->data.num);
            break;
            
        case NODE_VAR: {
            int offset = getVarOffset(node->data.name);
            if (offset == -1) {
                fprintf(stderr, "Error: Variable %s not declared\n", node->data.name);
                exit(1);
            }
            fprintf(output, "    lw $t%d, %d($sp)\n", getNextTemp(), offset);
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
            // Get array base offset from symbol table
            int baseOffset = getVarOffset(node->data.array_access.name);
            if (baseOffset == -1) {
                fprintf(stderr, "Error: Array %s not declared\n", node->data.array_access.name);
                exit(1);
            }
            // Generate code for index expression
            genExpr(node->data.array_access.index);
            int indexReg = tempReg - 1;
            
            // Calculate byte offset: index * 4 (each int is 4 bytes)
            fprintf(output, "    sll $t%d, $t%d, 2\n", indexReg, indexReg);  // Shift left by 2 = multiply by 4
            
            // Add base offset to get final address
            fprintf(output, "    addi $t%d, $sp, %d\n", getNextTemp(), baseOffset);
            int baseReg = tempReg - 1;
            fprintf(output, "    add $t%d, $t%d, $t%d\n", baseReg, baseReg, indexReg);
            
            // Load value from computed address
            fprintf(output, "    lw $t%d, 0($t%d)\n", indexReg, baseReg);
            tempReg = indexReg + 1;
            break;
        }
        case NODE_ARRAY_2D_ACCESS: {
            // Get array info from symbol table
            Symbol* sym = getSymbol(node->data.array_2d_access.name);
            if (!sym || !sym->is2DArray) {
                fprintf(stderr, "Error: 2D Array %s not declared\n", node->data.array_2d_access.name);
                exit(1);
            }
            
            int baseOffset = sym->offset;
            int cols = sym->cols;
            
            // Generate code for row index
            genExpr(node->data.array_2d_access.row);
            int rowReg = tempReg - 1;
            
            // Generate code for col index
            genExpr(node->data.array_2d_access.col);
            int colReg = tempReg - 1;
            
            // Calculate linear index: row * cols + col
            // First: row * cols
            fprintf(output, "    li $t%d, %d\n", getNextTemp(), cols);
            int colsReg = tempReg - 1;
            fprintf(output, "    mul $t%d, $t%d, $t%d\n", rowReg, rowReg, colsReg);
            
            // Then: add col
            fprintf(output, "    add $t%d, $t%d, $t%d\n", rowReg, rowReg, colReg);
            
            // Multiply by 4 for byte offset
            fprintf(output, "    sll $t%d, $t%d, 2\n", rowReg, rowReg);
            
            // Add base offset
            fprintf(output, "    addi $t%d, $sp, %d\n", colsReg, baseOffset);
            fprintf(output, "    add $t%d, $t%d, $t%d\n", colsReg, colsReg, rowReg);
            
            // Load value
            fprintf(output, "    lw $t%d, 0($t%d)\n", rowReg, colsReg);
            tempReg = rowReg + 1;
            break;
        }
        default:
            break;
    }
}

void genStmt(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_DECL: {
            int offset = addVar(node->data.name);
            if (offset == -1) {
                fprintf(stderr, "Error: Variable %s already declared\n", node->data.name);
                exit(1);
            }
            fprintf(output, "    # Declared %s at offset %d\n", node->data.name, offset);
            break;
        }

        case NODE_ARRAY_DECL: {
            int offset = addArray(node->data.array_decl.name, node->data.array_decl.size);
            if (offset == -1) {
                fprintf(stderr, "Error: Array %s already declared\n", node->data.array_decl.name);
                exit(1);
            }
            fprintf(output, "    # Declared array %s[%d] at offset %d\n", 
                    node->data.array_decl.name, node->data.array_decl.size, offset);
            break;
        }

        case NODE_ARRAY_2D_DECL: {
            int offset = addArray2D(node->data.array_2d_decl.name, 
                                     node->data.array_2d_decl.rows, 
                                     node->data.array_2d_decl.cols);
            if (offset == -1) {
                fprintf(stderr, "Error: 2D Array %s already declared\n", node->data.array_2d_decl.name);
                exit(1);
            }
            fprintf(output, "    # Declared 2D array %s[%d][%d] at offset %d\n", 
                    node->data.array_2d_decl.name, 
                    node->data.array_2d_decl.rows, 
                    node->data.array_2d_decl.cols, 
                    offset);
            break;
        }
        
        case NODE_ASSIGN: {
            int offset = getVarOffset(node->data.assign.var);
            if (offset == -1) {
                fprintf(stderr, "Error: Variable %s not declared\n", node->data.assign.var);
                exit(1);
            }
            genExpr(node->data.assign.value);
            fprintf(output, "    sw $t%d, %d($sp)\n", tempReg - 1, offset);
            tempReg = 0;
            break;
        }

        case NODE_ARRAY_ASSIGN: {
            // Get array base offset from symbol table
            int baseOffset = getVarOffset(node->data.array_assign.name);
            if (baseOffset == -1) {
                fprintf(stderr, "Error: Array %s not declared\n", node->data.array_assign.name);
                exit(1);
            }
            
            // Generate code for index expression
            genExpr(node->data.array_assign.index);
            int indexReg = tempReg - 1;
            
            // Calculate byte offset: index * 4
            fprintf(output, "    sll $t%d, $t%d, 2\n", indexReg, indexReg);
            
            // Add base offset to get final address
            fprintf(output, "    addi $t%d, $sp, %d\n", getNextTemp(), baseOffset);
            int baseReg = tempReg - 1;
            fprintf(output, "    add $t%d, $t%d, $t%d\n", baseReg, baseReg, indexReg);
            
            // Generate code for value expression
            genExpr(node->data.array_assign.value);
            int valueReg = tempReg - 1;
            
            // Store value at computed address
            fprintf(output, "    sw $t%d, 0($t%d)\n", valueReg, baseReg);
            tempReg = 0;
            break;
        }

        case NODE_ARRAY_2D_ASSIGN: {
            // Get array info from symbol table
            Symbol* sym = getSymbol(node->data.array_2d_assign.name);
            if (!sym || !sym->is2DArray) {
                fprintf(stderr, "Error: 2D Array %s not declared\n", node->data.array_2d_assign.name);
                exit(1);
            }
            
            int baseOffset = sym->offset;
            int cols = sym->cols;
            
            // Generate code for row index
            genExpr(node->data.array_2d_assign.row);
            int rowReg = tempReg - 1;
            
            // Generate code for col index
            genExpr(node->data.array_2d_assign.col);
            int colReg = tempReg - 1;
            
            // Calculate linear index: row * cols + col
            fprintf(output, "    li $t%d, %d\n", getNextTemp(), cols);
            int colsReg = tempReg - 1;
            fprintf(output, "    mul $t%d, $t%d, $t%d\n", rowReg, rowReg, colsReg);
            fprintf(output, "    add $t%d, $t%d, $t%d\n", rowReg, rowReg, colReg);
            
            // Multiply by 4 for byte offset
            fprintf(output, "    sll $t%d, $t%d, 2\n", rowReg, rowReg);
            
            // Add base offset
            fprintf(output, "    addi $t%d, $sp, %d\n", colsReg, baseOffset);
            fprintf(output, "    add $t%d, $t%d, $t%d\n", colsReg, colsReg, rowReg);
            
            // Generate value expression
            genExpr(node->data.array_2d_assign.value);
            int valueReg = tempReg - 1;
            
            // Store value
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
            
        case NODE_STMT_LIST:
            genStmt(node->data.stmtlist.stmt);
            genStmt(node->data.stmtlist.next);
            break;
            
        default:
            break;
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
    
    // MIPS program header
    fprintf(output, ".data\n");
    fprintf(output, "\n.text\n");
    fprintf(output, ".globl main\n");
    fprintf(output, "main:\n");
    
    // Allocate stack space (max 100 variables * 4 bytes)
    fprintf(output, "    # Allocate stack space\n");
    fprintf(output, "    addi $sp, $sp, -400\n\n");
    
    // Generate code for statements
    genStmt(root);
    
    // Program exit
    fprintf(output, "\n    # Exit program\n");
    fprintf(output, "    addi $sp, $sp, 400\n");
    fprintf(output, "    li $v0, 10\n");
    fprintf(output, "    syscall\n");
    
    fclose(output);
}