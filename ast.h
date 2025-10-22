#ifndef AST_H
#define AST_H

/* NODE TYPES - Different kinds of AST nodes in our language */
typedef enum {
    NODE_NUM,
    NODE_VAR,
    NODE_BINOP,
    NODE_DECL,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_STMT_LIST,
    NODE_ARRAY_DECL,
    NODE_ARRAY_ASSIGN,
    NODE_ARRAY_ACCESS,
    NODE_ARRAY_2D_DECL,
    NODE_ARRAY_2D_ASSIGN,
    NODE_ARRAY_2D_ACCESS,
    NODE_FUNC_DECL,
    NODE_FUNC_CALL,
    NODE_PARAM,
    NODE_PARAM_LIST,
    NODE_ARG_LIST,
    NODE_RETURN,
    NODE_FUNC_LIST
} NodeType;

/* AST NODE STRUCTURE */
typedef struct ASTNode {
    NodeType type;
    
    union {
        /* Literal number value */
        int num;
        
        /* Variable or declaration name */
        char* name;
        
        /* Binary operation structure */
        struct {
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binop;
        
        /* Assignment structure */
        struct {
            char* var;
            struct ASTNode* value;
        } assign;
        
        /* Array declaration */
        struct {
            char* name;
            int size;
        } array_decl;

        /* Array element assignment */
        struct {
            char* name;
            struct ASTNode* index;
            struct ASTNode* value;
        } array_assign;

        /* Array element access */
        struct {
            char* name;
            struct ASTNode* index;
        } array_access;

        /* 2D Array declaration */
        struct {
            char* name;
            int rows;
            int cols;
        } array_2d_decl;

        /* 2D Array element assignment */
        struct {
            char* name;
            struct ASTNode* row;
            struct ASTNode* col;
            struct ASTNode* value;
        } array_2d_assign;

        /* 2D Array element access */
        struct {
            char* name;
            struct ASTNode* row;
            struct ASTNode* col;
        } array_2d_access;
        
        /* Function declaration */
        struct {
            char* returnType;
            char* name;
            struct ASTNode* params;
            struct ASTNode* body;
        } func_decl;
        
        /* Function call */
        struct {
            char* name;
            struct ASTNode* args;
        } func_call;
        
        /* Parameter */
        struct {
            char* type;
            char* name;
        } param;
        
        /* Lists (params, args, functions, statements) */
        struct {
            struct ASTNode* item;
            struct ASTNode* next;
        } list;
        
        /* Return statement */
        struct ASTNode* return_expr;
        
        /* Print expression */
        struct ASTNode* expr;
        
        /* Statement list structure */
        struct {
            struct ASTNode* stmt;
            struct ASTNode* next;
        } stmtlist;
    } data;
} ASTNode;

/* AST CONSTRUCTION FUNCTIONS */
ASTNode* createNum(int value);
ASTNode* createVar(char* name);
ASTNode* createBinOp(char op, ASTNode* left, ASTNode* right);
ASTNode* createDecl(char* name);
ASTNode* createAssign(char* var, ASTNode* value);
ASTNode* createPrint(ASTNode* expr);
ASTNode* createStmtList(ASTNode* stmt1, ASTNode* stmt2);
ASTNode* createArrayDecl(char* name, int size);
ASTNode* createArrayAssign(char* name, ASTNode* index, ASTNode* value);
ASTNode* createArrayAccess(char* name, ASTNode* index);
ASTNode* createArray2DDecl(char* name, int rows, int cols);
ASTNode* createArray2DAssign(char* name, ASTNode* row, ASTNode* col, ASTNode* value);
ASTNode* createArray2DAccess(char* name, ASTNode* row, ASTNode* col);
ASTNode* createFuncDecl(char* returnType, char* name, ASTNode* params, ASTNode* body);
ASTNode* createFuncCall(char* name, ASTNode* args);
ASTNode* createParam(char* type, char* name);
ASTNode* createParamList(ASTNode* param, ASTNode* next);
ASTNode* createArgList(ASTNode* arg, ASTNode* next);
ASTNode* createReturn(ASTNode* expr);
ASTNode* createFuncList(ASTNode* func, ASTNode* next);

/* AST DISPLAY FUNCTION */
void printAST(ASTNode* node, int level);

#endif