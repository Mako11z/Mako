#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <map>

class CodeGenerator
{
public:
    CodeGenerator();
    ~CodeGenerator();
    void generateCode(ASTNode *root);
    void printIR(const std::string &filename);
    void printIR();

private:
    llvm::LLVMContext context;
    llvm::Module *module;
    llvm::IRBuilder<> builder;
    llvm::Function *mainFunction;
    std::map<std::string, llvm::Value *> namedValues;
    std::map<std::string, llvm::Value *> identifierToValue;
    llvm::Value *generateCodeForNode(ASTNode *node);
    llvm::Value *generateCodeForCondition(ConditionalNode *node);
    llvm::Value *generateCodeForVariableDef(VariableDefNode *node);
    llvm::Value *generateCodeForLiteral(LiteralNode *node);
    llvm::Value *generateCodeForIfStatement(IfStatementNode *node);
    llvm::Value *generateCodeForWhileLoop(WhileLoopNode *node);
    llvm::Value *generateCodeForAssignment(AssignmentNode *node);
    llvm::Value *generateCodeForArithmic(ArithmicNode *node);
    llvm::Value *generateCodeForIdentifier(IdentifierNode *node);
    llvm::Value *combineConditions(const std::vector<ASTNode *> &conditions,
                                   const std::vector<std::string> &logicalOps);
};

#endif