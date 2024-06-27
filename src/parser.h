#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include "lexer.h"

enum class ASTNodeType
{
    BinaryExpression,
    VariableDef,
    Literal,
    IfStatement,
    WhileLoop,
    Assignment,
    Print
};

struct ASTNode
{
    ASTNodeType type;
    std::vector<ASTNode *> children;
    ASTNode(ASTNodeType t) : type(t) {}
    virtual ~ASTNode() = default;
};

struct VariableDef : ASTNode
{
    std::string data_type;
    std::string var_name;
    VariableDef(std::string &type, std::string &name) : ASTNode(ASTNodeType::VariableDef), data_type(type), var_name(name) {}
};

struct BinaryExpressionNode : ASTNode
{
    std::string operation;
    BinaryExpressionNode(std::string &op, ASTNode *l, ASTNode *r) : ASTNode(ASTNodeType::BinaryExpression), operation(op)
    {
        children.push_back(l);
        children.push_back(r);
    }
};

struct LiteralNode : ASTNode
{
    std::string value;
    LiteralNode(std::string &val) : ASTNode(ASTNodeType::Literal), value(val) {}
};

struct AssignmemtNode : ASTNode
{
    AssignmemtNode(ASTNode *l, ASTNode *r) : ASTNode(ASTNodeType::Assignment)
    {
        children.push_back(l);
        children.push_back(r);
    }
};

struct IfStatementNode : ASTNode
{
    std::vector<ASTNode *> conditions;
    std::vector<ASTNode *> body;
    IfStatementNode() : ASTNode(ASTNodeType::IfStatement) {}
};

struct WhileLoopNode : ASTNode
{
    std::vector<ASTNode *> conditions;
    std::vector<ASTNode *> body;
    WhileLoopNode() : ASTNode(ASTNodeType::WhileLoop) {}
};

struct PrintNode : ASTNode
{
    std::string value_to_print;
    PrintNode(std::string &val) : ASTNode(ASTNodeType::Print), value_to_print(val) {}
};

class Parser
{
public:
    Parser(std::vector<Token> &tokens) : tokens(tokens), current_index(0) {}
    ASTNode *parse();

private:
    std::vector<Token> &tokens;
    size_t current_index;
    Token peekToken()
    {
        if (current_index + 1 < tokens.size())
        {
            return tokens[current_index + 1];
        }
        else
        {
            return Token{TokenType::Punctuation, ""};
        }
    }
    void advanceToken()
    {
        current_index++;
    }
};

#endif