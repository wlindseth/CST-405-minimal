/* AST IMPLEMENTATION - FIXED FOR FUNCTIONS
 * Functions to create and manipulate Abstract Syntax Tree nodes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Create a function declaration node */
ASTNode* createFuncDecl(char* returnType, char* name, ASTNode* params, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNC_DECL;
    node->data.func_decl.returnType = strdup(returnType);
    node->data.func_decl.name = strdup(name);
    node->data.func_decl.params = params;
    node->data.func_decl.body = body;
    return node;
}

/* Create a function call node */
ASTNode* createFuncCall(char* name, ASTNode* args) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNC_CALL;
    node->data.func_call.name = strdup(name);
    node->data.func_call.args = args;
    return node;
}

/* Create a parameter node */
ASTNode* createParam(char* type, char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PARAM;
    node->data.param.type = strdup(type);
    node->data.param.name = strdup(name);
    return node;
}

/* Create a parameter list node */
ASTNode* createParamList(ASTNode* param, ASTNode* next) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PARAM_LIST;
    node->data.list.item = param;
    node->data.list.next = next;
    return node;
}

/* Create an argument list node */
ASTNode* createArgList(ASTNode* arg, ASTNode* next) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARG_LIST;
    node->data.list.item = arg;
    node->data.list.next = next;
    return node;
}

/* Create a return statement node */
ASTNode* createReturn(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_RETURN;
    node->data.return_expr = expr;
    return node;
}

/* Create a function list node */
ASTNode* createFuncList(ASTNode* func, ASTNode* next) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNC_LIST;
    node->data.list.item = func;
    node->data.list.next = next;
    return node;
}

/* Create a number literal node */
ASTNode* createNum(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_NUM;
    node->data.num = value;
    return node;
}

/* Create a variable reference node */
ASTNode* createVar(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_VAR;
    node->data.name = strdup(name);
    return node;
}

/* Create a binary operation node */
ASTNode* createBinOp(char op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BINOP;
    node->data.binop.op = op;
    node->data.binop.left = left;
    node->data.binop.right = right;
    return node;
}

/* Create a variable declaration node */
ASTNode* createDecl(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_DECL;
    node->data.name = strdup(name);
    return node;
}

/* Create an assignment statement node */
ASTNode* createAssign(char* var, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ASSIGN;
    node->data.assign.var = strdup(var);
    node->data.assign.value = value;
    return node;
}

/* Create a print statement node */
ASTNode* createPrint(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PRINT;
    node->data.expr = expr;
    return node;
}

/* Create an array declaration node */
ASTNode* createArrayDecl(char* name, int size) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_DECL;
    node->data.array_decl.name = strdup(name);
    node->data.array_decl.size = size;
    return node;
}

/* Create an array element assignment node */
ASTNode* createArrayAssign(char* name, ASTNode* index, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_ASSIGN;
    node->data.array_assign.name = strdup(name);
    node->data.array_assign.index = index;
    node->data.array_assign.value = value;
    return node;
}

/* Create an array element access node */
ASTNode* createArrayAccess(char* name, ASTNode* index) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_ACCESS;
    node->data.array_access.name = strdup(name);
    node->data.array_access.index = index;
    return node;
}

/* Create a 2D array declaration node */
ASTNode* createArray2DDecl(char* name, int rows, int cols) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_2D_DECL;
    node->data.array_2d_decl.name = strdup(name);
    node->data.array_2d_decl.rows = rows;
    node->data.array_2d_decl.cols = cols;
    return node;
}

/* Create a 2D array element assignment node */
ASTNode* createArray2DAssign(char* name, ASTNode* row, ASTNode* col, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_2D_ASSIGN;
    node->data.array_2d_assign.name = strdup(name);
    node->data.array_2d_assign.row = row;
    node->data.array_2d_assign.col = col;
    node->data.array_2d_assign.value = value;
    return node;
}

/* Create a 2D array element access node */
ASTNode* createArray2DAccess(char* name, ASTNode* row, ASTNode* col) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_2D_ACCESS;
    node->data.array_2d_access.name = strdup(name);
    node->data.array_2d_access.row = row;
    node->data.array_2d_access.col = col;
    return node;
}

/* Create a statement list node */
ASTNode* createStmtList(ASTNode* stmt1, ASTNode* stmt2) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_STMT_LIST;
    node->data.stmtlist.stmt = stmt1;
    node->data.stmtlist.next = stmt2;
    return node;
}

/* Display the AST structure */
void printAST(ASTNode* node, int level) {
    if (!node) return;
    
    for (int i = 0; i < level; i++) printf("  ");
    
    switch(node->type) {
        case NODE_NUM:
            printf("NUM: %d\n", node->data.num);
            break;
        case NODE_VAR:
            printf("VAR: %s\n", node->data.name);
            break;
        case NODE_BINOP:
            printf("BINOP: %c\n", node->data.binop.op);
            printAST(node->data.binop.left, level + 1);
            printAST(node->data.binop.right, level + 1);
            break;
        case NODE_DECL:
            printf("DECL: %s\n", node->data.name);
            break;
        case NODE_ASSIGN:
            printf("ASSIGN TO: %s\n", node->data.assign.var);
            printAST(node->data.assign.value, level + 1);
            break;
        case NODE_PRINT:
            printf("PRINT\n");
            printAST(node->data.expr, level + 1);
            break;
        case NODE_STMT_LIST:
            printAST(node->data.stmtlist.stmt, level);
            printAST(node->data.stmtlist.next, level);
            break;
        case NODE_ARRAY_DECL:
            printf("ARRAY_DECL: %s[%d]\n", node->data.array_decl.name, node->data.array_decl.size);
            break;
        case NODE_ARRAY_ASSIGN:
            printf("ARRAY_ASSIGN TO: %s\n", node->data.array_assign.name);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Index:\n");
            printAST(node->data.array_assign.index, level + 2);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Value:\n");
            printAST(node->data.array_assign.value, level + 2);
            break;
        case NODE_ARRAY_ACCESS:
            printf("ARRAY_ACCESS: %s\n", node->data.array_access.name);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Index:\n");
            printAST(node->data.array_access.index, level + 2);
            break;
        case NODE_ARRAY_2D_DECL:
            printf("ARRAY_2D_DECL: %s[%d][%d]\n", 
                   node->data.array_2d_decl.name, 
                   node->data.array_2d_decl.rows, 
                   node->data.array_2d_decl.cols);
            break;
        case NODE_ARRAY_2D_ASSIGN:
            printf("ARRAY_2D_ASSIGN TO: %s\n", node->data.array_2d_assign.name);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Row:\n");
            printAST(node->data.array_2d_assign.row, level + 2);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Col:\n");
            printAST(node->data.array_2d_assign.col, level + 2);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Value:\n");
            printAST(node->data.array_2d_assign.value, level + 2);
            break;
        case NODE_ARRAY_2D_ACCESS:
            printf("ARRAY_2D_ACCESS: %s\n", node->data.array_2d_access.name);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Row:\n");
            printAST(node->data.array_2d_access.row, level + 2);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Col:\n");
            printAST(node->data.array_2d_access.col, level + 2);
            break;
        case NODE_FUNC_DECL:
            printf("FUNC_DECL: %s %s\n", node->data.func_decl.returnType, node->data.func_decl.name);
            if (node->data.func_decl.params) {
                for (int i = 0; i < level + 1; i++) printf("   ");
                printf("Parameters:\n");
                printAST(node->data.func_decl.params, level + 2);
            }
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Body:\n");
            printAST(node->data.func_decl.body, level + 2);
            break;
        case NODE_FUNC_CALL:
            printf("FUNC_CALL: %s\n", node->data.func_call.name);
            if (node->data.func_call.args) {
                for (int i = 0; i < level + 1; i++) printf("   ");
                printf("Arguments:\n");
                printAST(node->data.func_call.args, level + 2);
            }
            break;
        case NODE_PARAM:
            printf("PARAM: %s %s\n", node->data.param.type, node->data.param.name);
            break;
        case NODE_PARAM_LIST:
        case NODE_ARG_LIST:
            printAST(node->data.list.item, level);
            printAST(node->data.list.next, level);
            break;
        case NODE_RETURN:
            printf("RETURN\n");
            if (node->data.return_expr) {
                printAST(node->data.return_expr, level + 1);
            }
            break;
        case NODE_FUNC_LIST:
            printAST(node->data.list.item, level);
            printAST(node->data.list.next, level);
            break;
    }
}