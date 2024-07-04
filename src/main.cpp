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
    case ASTNodeType::IfStatement:
    {
        IfStatementNode *ifNode = dynamic_cast<IfStatementNode *>(node);
        std::cout << indentStr << "IfStatementNode\n";
        std::cout << indentStr << "  Conditions:\n";
        for (ASTNode *condition : ifNode->conditions)
        {
            printAST(condition, indent + 4);
        }
        if (ifNode->body)
        {
            std::cout << indentStr << "  Body:\n";
            printAST(ifNode->body, indent + 4);
        }
        if (ifNode->elseNode)
        {
            std::cout << indentStr << "  Else:\n";
            printAST(ifNode->elseNode, indent + 4);
        }
    }
    break;
    case ASTNodeType::ElseStatement:
    {
        ElseNode *elseNode = dynamic_cast<ElseNode *>(node);
        std::cout << indentStr << "ElseNode\n";
        if (elseNode->body)
        {
            std::cout << indentStr << "  Body:\n";
            printAST(elseNode->body, indent + 4);
        }
    }
    break;
    case ASTNodeType::WhileLoop:
    {
        WhileLoopNode *whileNode = dynamic_cast<WhileLoopNode *>(node);
        std::cout << indentStr << "WhileLoopNode\n";
        std::cout << indentStr << "  Conditions:\n";
        for (ASTNode *condition : whileNode->conditions)
        {
            printAST(condition, indent + 4);
        }
        if (whileNode->body)
        {
            std::cout << indentStr << "  Body:\n";
            printAST(whileNode->body, indent + 4);
        }
    }
    break;
    case ASTNodeType::Print:
    {
        PrintNode *printNode = dynamic_cast<PrintNode *>(node);
        std::cout << indentStr << "PrintNode(value_to_print: " << printNode->value_to_print << ")\n";
    }
    break;
    case ASTNodeType::ConditionalExpression:
    {
        ConditionalNode *condNode = dynamic_cast<ConditionalNode *>(node);
        std::cout << indentStr << "ConditionalNode(operation: ";
        if (!condNode->operation.empty())
            std::cout << condNode->operation;
        else
            std::cout << "empty";
        std::cout << ")\n";

        std::cout << indentStr << "  Left:\n";
        printAST(condNode->children[0], indent + 4);

        std::cout << indentStr << "  Right:\n";
        printAST(condNode->children[1], indent + 4);
    }
    break;
    case ASTNodeType::ArithmicExpressions:
    {
        ArithmicNode *arithNode = dynamic_cast<ArithmicNode *>(node);
        std::cout << indentStr << "ArithmicNode(operation: " << arithNode->operation << ")\n";
        std::cout << indentStr << "  Left:\n";
        printAST(arithNode->children[0], indent + 4);
        std::cout << indentStr << "  Right:\n";
        printAST(arithNode->children[1], indent + 4);
    }
    break;
    case ASTNodeType::BodyStatements:
    {
        BodyNode *bodyNode = dynamic_cast<BodyNode *>(node);
        std::cout << indentStr << "BodyNode\n";
        for (ASTNode *statement : bodyNode->statements)
        {
            printAST(statement, indent + 2);
        }
    }
    break;
    case ASTNodeType::Identifier:
    {
        IdentifierNode *idNode = dynamic_cast<IdentifierNode *>(node);
        std::cout << indentStr << "IdentifierNode(name: " << idNode->name << ")\n";
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
    /*
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
            case TokenType::LogicalOperator:
                std::cout << "Logical Operator: ";
                break;
            }
            std::cout << token.value << std::endl;
        }
        */
    ASTNode *root = p.parse();
    printAST(root);

    return 0;
}