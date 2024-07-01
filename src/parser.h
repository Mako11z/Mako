#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include "lexer.h"

enum class ASTNodeType
{
    Program,
    VariableDef,
    Literal,
    IfStatement,
    WhileLoop,
    Assignment,
    Print,
    ConditionalExpression,
    ArithmicExpressions
};

struct ASTNode
{
    ASTNodeType type;
    std::vector<ASTNode *> children;
    ASTNode(ASTNodeType t) : type(t) {}
    virtual ~ASTNode() = default;
};

struct ProgramNode : ASTNode
{
    ProgramNode() : ASTNode(ASTNodeType::Program) {}
};

struct ConditionalNode : ASTNode
{
    std::string operation;
    ConditionalNode(const std::string &op, ASTNode *left, ASTNode *right) : ASTNode(ASTNodeType::ConditionalExpression)
    {
        children.push_back(left);
        children.push_back(right);
    }
};

struct ArithmicNode : ASTNode
{
    std::string operation;
    ArithmicNode(const std::string op, ASTNode *left, ASTNode *right) : ASTNode(ASTNodeType::ArithmicExpressions)
    {
        children.push_back(left);
        children.push_back(right);
    }
};

struct VariableDefNode : ASTNode
{
    std::string data_type;
    std::string var_name;
    VariableDefNode(std::string &type, std::string &name) : ASTNode(ASTNodeType::VariableDef), data_type(type), var_name(name) {}
};

struct LiteralNode : ASTNode
{
    std::string value;
    LiteralNode(std::string &val) : ASTNode(ASTNodeType::Literal), value(val) {}
};

struct AssignmentNode : ASTNode
{
    AssignmentNode(ASTNode *left, ASTNode *right) : ASTNode(ASTNodeType::Assignment)
    {
        children.push_back(left);
        children.push_back(right);
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
    Token getCurrentToken();
    Token peekToken();
    void advanceToken();
    bool checkForEndOfFile();
    ASTNode *parseIfStatement();
    ASTNode *parseVariableDef();
    ASTNode *parseWhileLoop();
    ASTNode *parseAssignment(VariableDefNode *);
    ASTNode *parseLiteral();
};

#endif