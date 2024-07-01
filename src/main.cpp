#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "parser.h"
#include "lexer.h"

void printAST(ASTNode *node, int indent = 0)
{
    if (node == nullptr)
        return;

    std::string indentStr(indent, ' ');
    switch (node->type)
    {
    case ASTNodeType::Program:
        std::cout << indentStr << "ProgramNode\n";
        for (ASTNode *child : node->children)
        {
            printAST(child, indent + 2);
        }
        break;
    case ASTNodeType::VariableDef:
    {
        VariableDefNode *varDef = dynamic_cast<VariableDefNode *>(node);
        std::cout << indentStr << "VariableDefNode(data_type: " << varDef->data_type << ", var_name: " << varDef->var_name << ")\n";
    }
    break;
    case ASTNodeType::Literal:
    {
        LiteralNode *literal = dynamic_cast<LiteralNode *>(node);
        std::cout << indentStr << "LiteralNode(value: " << literal->value << ")\n";
    }
    break;
    case ASTNodeType::Assignment:
        std::cout << indentStr << "AssignmentNode\n";
        for (ASTNode *child : node->children)
        {
            printAST(child, indent + 2);
        }
        break;
    case ASTNodeType::BinaryExpression:
    {
        BinaryExpressionNode *binaryExpr = dynamic_cast<BinaryExpressionNode *>(node);
        std::cout << indentStr << "BinaryExpressionNode(operation: " << binaryExpr->operation << ")\n";
        for (ASTNode *child : node->children)
        {
            printAST(child, indent + 2);
        }
    }
    break;
    case ASTNodeType::IfStatement:
        std::cout << indentStr << "IfStatementNode\n";
        for (ASTNode *child : node->children)
        {
            printAST(child, indent + 2);
        }
        break;
    case ASTNodeType::WhileLoop:
        std::cout << indentStr << "WhileLoopNode\n";
        for (ASTNode *child : node->children)
        {
            printAST(child, indent + 2);
        }
        break;
    case ASTNodeType::Print:
    {
        PrintNode *printNode = dynamic_cast<PrintNode *>(node);
        std::cout << indentStr << "PrintNode(value_to_print: " << printNode->value_to_print << ")\n";
    }
    break;
    default:
        std::cout << indentStr << "Unknown ASTNode type\n";
        break;
    }
}

int main()
{
    std::string file_name = "test.txt";
    Lexer lexer(file_name);

    std::vector<Token> tokens = lexer.tokenize();
    Parser p = (tokens);

    for (const Token &token : tokens)
    {
        // Print token type
        switch (token.type)
        {
        case TokenType::Keyword:
            std::cout << "Keyword: ";
            break;
        case TokenType::Identifier:
            std::cout << "Identifier: ";
            break;
        case TokenType::Literal:
            std::cout << "Literal: ";
            break;
        case TokenType::Operator:
            std::cout << "Operator: ";
            break;
        case TokenType::Punctuation:
            std::cout << "Punctuation: ";
            break;
        case TokenType::ComparisonOperator:
            std::cout << "Comparison Operator: ";
            break;
        case TokenType::AssignmentOperator:
            std::cout << "Assignment Operator: ";
            break;
        }
        std::cout << token.value << std::endl;
    }
    ASTNode *root = p.parse();
    printAST(root);

    return 0;
}