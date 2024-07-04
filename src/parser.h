#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <set>
#include "lexer.h"

enum class ASTNodeType
{
    Program,
    VariableDef,
    Literal,
    IfStatement,
    ElseStatement,
    WhileLoop,
    Assignment,
    Print,
    ConditionalExpression,
    ArithmicExpressions,
    BodyStatements,
    Identifier,
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

struct BodyNode : ASTNode
{
    std::vector<ASTNode *> statements;
    BodyNode() : ASTNode(ASTNodeType::BodyStatements) {}
};

struct ConditionalNode : ASTNode
{
    std::string operation;
    ConditionalNode(const std::string &op, ASTNode *left, ASTNode *right) : ASTNode(ASTNodeType::ConditionalExpression), operation(op)
    {
        children.push_back(left);
        children.push_back(right);
    }
};

struct IdentifierNode : ASTNode
{
    std::string name;
    IdentifierNode(const std::string &id) : ASTNode(ASTNodeType::Identifier), name(id) {}
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

struct ElseNode : ASTNode
{
    BodyNode *body;
    ElseNode() : ASTNode(ASTNodeType::ElseStatement), body(nullptr) {}
};

struct IfStatementNode : ASTNode
{
    std::vector<ASTNode *> conditions;
    BodyNode *body;
    ASTNode *elseNode;
    IfStatementNode() : ASTNode(ASTNodeType::IfStatement), body(nullptr), elseNode(nullptr) {}
};

struct WhileLoopNode : ASTNode
{
    std::vector<ASTNode *> conditions;
    BodyNode *body;
    WhileLoopNode() : ASTNode(ASTNodeType::WhileLoop), body(nullptr) {}
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
    // Variables
    std::vector<Token> &tokens;
    std::set<std::string> defined_variables;
    size_t current_index;
    // Functions
    template <typename NodeType>
    NodeType *getConditions();

    Token getCurrentToken();
    Token peekToken();
    bool advanceAndCheckEOF();
    void advanceToken();
    bool checkForEndOfFile();
    bool checkForDefinedVar(const std::string &);
    ASTNode *parseIfStatement();
    ASTNode *parseVariableDef();
    ASTNode *parseWhileLoop();
    ASTNode *parseAssignment(VariableDefNode *);
    ASTNode *parseCondition();
    ASTNode *getNextStatement();
    BodyNode *getBodyStatements();
};

#endif