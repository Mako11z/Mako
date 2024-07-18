#include "codegen.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

/*
    Create a main function and set it as the entry point of the basic block (entry)
*/
CodeGenerator::CodeGenerator()
    : module(new llvm::Module("main", context)), builder(context)
{
    // Create a main function: int main() {}
    llvm::FunctionType *funcType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    mainFunction = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", mainFunction);
    builder.SetInsertPoint(entry);
}

// Deconstructor
CodeGenerator::~CodeGenerator()
{
    delete module;
}

// Starts code generation from the root and create a return statement for the main function
void CodeGenerator::generateCode(ASTNode *root)
{
    generateCodeForNode(root);
    builder.CreateRet(llvm::ConstantInt::get(context, llvm::APInt(32, 0)));
}

// Generate LLVM IR for each node in the AST
llvm::Value *CodeGenerator::generateCodeForNode(ASTNode *node)
{
    if (!node)
    {
        std::cerr << "Null node encountered\n";
        return nullptr;
    }
    switch (node->type)
    {
    case ASTNodeType::Program:
        for (ASTNode *child : node->children)
        {
            std::cout << "Iterating through prgram node \n";
            generateCodeForNode(child);
            std::cout << "Im here after call \n";
        }
        break;
    case ASTNodeType::VariableDef:
        std::cout << "Iterating through variable def in generate code function \n";
        return generateCodeForVariableDef(static_cast<VariableDefNode *>(node));
    case ASTNodeType::Assignment:
        std::cout << "Generating code for Assignment node\n";
        return generateCodeForAssignment(static_cast<AssignmentNode *>(node));
    case ASTNodeType::Literal:
        std::cout << "Creating memory for literal \n";
        return generateCodeForLiteral(static_cast<LiteralNode *>(node));
    case ASTNodeType::IfStatement:
        std::cout << "Calling if statement \n";
        return generateCodeForIfStatement(static_cast<IfStatementNode *>(node));
    case ASTNodeType::BodyStatements:
        for (ASTNode *stmt : static_cast<BodyNode *>(node)->statements)
        {
            generateCodeForNode(stmt);
        }
        break;
    case ASTNodeType::ConditionalExpression:
        return generateCodeForCondition(static_cast<ConditionalNode *>(node));
    case ASTNodeType::Identifier: // Handle Identifier
        return generateCodeForIdentifier(static_cast<IdentifierNode *>(node));
    default:
        std::cout << "unknow statemnet \n";
        return nullptr;
    }
    return nullptr;
}
llvm::Value *CodeGenerator::generateCodeForCondition(ConditionalNode *node)
{
    std::cout << "Im here in the condition function \n";
    llvm::Value *left = generateCodeForNode(node->children[0]);

    // If left is an IdentifierNode, fetch its associated literal value
    if (auto *leftIdentifier = dynamic_cast<IdentifierNode *>(node->children[0]))
    {
        if (identifierToValue.count(leftIdentifier->name))

            left = identifierToValue[leftIdentifier->name];
    }

    llvm::Value *right = generateCodeForNode(node->children[1]);
    std::cout << "Operation: " << node->operation << std::endl;

    if (left->getType() != right->getType())
    {
        std::cout << "NOTTT EQUALLL \n";
        std::string leftTypeStr;
        llvm::raw_string_ostream leftStream(leftTypeStr);
        left->getType()->print(leftStream);

        std::string rightTypeStr;
        llvm::raw_string_ostream rightStream(rightTypeStr);
        right->getType()->print(rightStream);

        std::cout << "Left type: " << leftStream.str() << std::endl;
        std::cout << "Right type: " << rightStream.str() << std::endl;
        return nullptr;
    }
    std::string leftTypeStr;
    llvm::raw_string_ostream leftStream(leftTypeStr);
    left->getType()->print(leftStream);

    std::string rightTypeStr;
    llvm::raw_string_ostream rightStream(rightTypeStr);
    right->getType()->print(rightStream);

    std::cout << "Left type: " << leftStream.str() << std::endl;
    std::cout << "Right type: " << rightStream.str() << std::endl;

    if (node->operation == "<")
    {
        std::cout << "Im here in the condition where op = < \n";
        return builder.CreateICmpSLT(left, right, "cmptmp");
    }
    else if (node->operation == "<=")
    {
        return builder.CreateICmpSLE(left, right, "cmptmp");
    }
    else if (node->operation == ">")
    {
        return builder.CreateICmpSGT(left, right, "cmptmp");
    }
    else if (node->operation == ">=")
    {
        return builder.CreateICmpSGE(left, right, "cmptmp");
    }
    else if (node->operation == "==")
    {
        return builder.CreateICmpEQ(left, right, "cmptmp");
    }
    else if (node->operation == "!=")
    {
        return builder.CreateICmpNE(left, right, "cmptmp");
    }
    else
    {
        std::cerr << "Unknown operation in ConditionalNode: " << node->operation << "\n";
        return nullptr;
    }
}
llvm::Value *CodeGenerator::generateCodeForIfStatement(IfStatementNode *node)
{
    llvm::Value *combinedCondition = generateCodeForNode(node->conditions[0]);

    // Combine all conditions with logical AND
    for (size_t i = 1; i < node->conditions.size(); ++i)
    {
        llvm::Value *nextCondition = generateCodeForNode(node->conditions[i]);
        std::string logicalOp = node->logicalOps[i - 1];
        if (logicalOp == "and")
        {
            combinedCondition = builder.CreateAnd(combinedCondition, nextCondition, "andtmp");
        }
        else if (logicalOp == "or")
        {
            combinedCondition = builder.CreateOr(combinedCondition, nextCondition, "ortmp");
        }
    }

    llvm::BasicBlock *ifBB = llvm::BasicBlock::Create(context, "if", mainFunction);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "merge", mainFunction);

    // Conditional branch based on combined condition
    builder.CreateCondBr(combinedCondition, ifBB, mergeBB);

    // Set insertion point for 'if' block
    builder.SetInsertPoint(ifBB);
    generateCodeForNode(node->body);
    builder.CreateBr(mergeBB);

    // Set insertion point for merge block
    builder.SetInsertPoint(mergeBB);

    return nullptr;
}

// Handle VariableDefNodes and allocate memory for the variable
llvm::Value *CodeGenerator::generateCodeForVariableDef(VariableDefNode *node)
{
    // Determine data type
    llvm::Type *type;
    if (node->data_type == "int")
    {
        type = llvm::Type::getInt32Ty(context);
    }
    else if (node->data_type == "float")
    {
        type = llvm::Type::getFloatTy(context);
    }
    else
    {
        return nullptr;
    }
    // Allocate memory of the variable
    llvm::Value *alloc = builder.CreateAlloca(type, 0, node->var_name.c_str());
    namedValues[node->var_name] = alloc;
    return alloc;
}

// Generates LLVM IR for LiteralNodes
llvm::Value *CodeGenerator::generateCodeForLiteral(LiteralNode *node)
{
    if (node->value.find('.') != std::string::npos)
    {
        // Float
        return llvm::ConstantFP::get(context, llvm::APFloat(std::stof(node->value)));
    }
    else
    {
        // Integer
        return llvm::ConstantInt::get(context, llvm::APInt(32, std::stoi(node->value)));
    }
}

// Handle AsisgnmentNodes
// Generate LLVM IR for the left-hand side and right-hand side children of the assignment
llvm::Value *CodeGenerator::generateCodeForAssignment(AssignmentNode *node)
{
    llvm::Value *left = generateCodeForNode(node->children[0]);
    llvm::Value *right = generateCodeForNode(node->children[1]);

    VariableDefNode *var_node = static_cast<VariableDefNode *>(node->children[0]);
    identifierToValue[var_node->var_name] = right;

    builder.CreateStore(right, left);
    return right;
}

llvm::Value *CodeGenerator::generateCodeForIdentifier(IdentifierNode *node)
{
    llvm::Value *val = namedValues[node->name];
    if (!val)
    {
        std::cerr << "Unknown variable name: " << node->name << "\n";
        return nullptr;
    }
    // Retrieve the LLVM type of the value
    return val;
}

void CodeGenerator::printIR()
{
    module->print(llvm::outs(), nullptr);
}
void CodeGenerator::printIR(const std::string &filename)
{
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OpenFlags());
    if (EC)
    {
        llvm::errs() << "Could not open file: " << EC.message();
        return;
    }
    module->print(dest, nullptr);
    dest.close();
}