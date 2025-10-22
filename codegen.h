#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

void generateMIPS(ASTNode* root, const char* filename);
int countLocalVars(ASTNode* node);

#endif