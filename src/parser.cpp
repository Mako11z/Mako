#include <iostream>
#include <string>
#include <vector>
#include "parser.h"

ASTNode *Parser::parse()
{
    ProgramNode *program = new ProgramNode();
    while (!checkForEndOfFile())
    {
        ASTNode *node = getNextStatement();
        if (node == nullptr)
        {
            break;
        }
        program->children.push_back(node);
    }

    return program;
}

bool Parser::advanceAndCheckEOF()
{
    advanceToken();
    if (checkForEndOfFile())
    {
        std::cout << "End of File \n";
        return true;
    }
    else
        return false;
}

bool Parser::checkForEndOfFile()
{
    return current_index > tokens.size();
}

/*
    Check to see if the variable has been defined
*/
bool Parser::checkForDefinedVar(const std::string &name)
{
    return defined_variables.find(name) != defined_variables.end();
}

template <typename NodeType>
NodeType *Parser::getConditions()
{
    NodeType *node = new NodeType();
    // Loop until end of condition
    Token current_token = getCurrentToken();
    while (current_token.value != ")")
    {
        ASTNode *condition = parseCondition();
        if (condition == nullptr)
        {
            std::cout << "Error parsing condition \n";
            delete node;
            return nullptr;
        }
        node->conditions.push_back(condition);

        current_token = getCurrentToken();
        if (current_token.value == ")")
        {
            break;
        }
        else if (current_token.type == TokenType::LogicalOperator)
        {
            // Advance
            if (advanceAndCheckEOF())
                return nullptr;
        }
        else
        {
            delete node;
            std::cout << "Expected a logical operator or ') \n";
            return nullptr;
        }
        current_token = getCurrentToken();
    }
    if (advanceAndCheckEOF())
        return nullptr;
    return node;
}

BodyNode *Parser::getBodyStatements()
{
    // Advance past '{'
    if (advanceAndCheckEOF())
        return nullptr;
    Token current_token = getCurrentToken();
    BodyNode *body_node = new BodyNode();
    while (current_token.value != "}")
    {
        ASTNode *statement = getNextStatement();
        if (statement == nullptr)
        {
            std::cout << "Error parsing in body \n";
            delete body_node;
            return nullptr;
        }
        body_node->statements.push_back(statement);
        current_token = getCurrentToken();
        if (current_token.value == "}")
        {
            break;
        }
    }
    return body_node;
}

ASTNode *Parser::parseIfStatement()
{
    // Skip the 'if' keyword and check for nullptr
    if (advanceAndCheckEOF())
        return nullptr;
    Token current_token = getCurrentToken();
    if (current_token.value != "(")
    {
        std::cout << "Syntax Error, Expected '(' " << std::endl;
        return nullptr;
    }
    // Advance past '('
    if (advanceAndCheckEOF())
        return nullptr;

    IfStatementNode *if_node = getConditions<IfStatementNode>();
    if (if_node == nullptr)
        return nullptr;

    BodyNode *body_node = getBodyStatements();
    current_token = getCurrentToken();
    if (current_token.value != "}")
    {
        std::cout << "Expected '}' \n";
        return nullptr;
    }
    if (advanceAndCheckEOF())
        return nullptr;

    if_node->body = body_node;
    return if_node;
}

/*
    Parse while loop
*/
ASTNode *Parser::parseWhileLoop()
{
    // Skip the 'while' keyword and check for nullptr
    if (advanceAndCheckEOF())
        return nullptr;
    Token current_token = getCurrentToken();
    if (current_token.value != "(")
    {
        std::cout << "Syntax Error, Expected '(' " << std::endl;
        return nullptr;
    }

    if (advanceAndCheckEOF())
        return nullptr;

    WhileLoopNode *while_node = getConditions<WhileLoopNode>();
    if (while_node == nullptr)
        return nullptr;

    BodyNode *body_node = getBodyStatements();
    current_token = getCurrentToken();
    if (current_token.value != "}")
    {
        std::cout << "Expected '}' \n";
        return nullptr;
    }
    if (advanceAndCheckEOF())
        return nullptr;

    while_node->body = body_node;
    return while_node;
}

// Parse a condition and grab the left operand, operator, and right operand
ASTNode *Parser::parseCondition()
{
    Token current_token = getCurrentToken();
    std::string op;
    ASTNode *left_child = nullptr;

    // Left child
    if (current_token.type == TokenType::Identifier)
    {
        left_child = new IdentifierNode(current_token.value);
    }
    else if (current_token.type == TokenType::Literal)
    {
        left_child = new LiteralNode(current_token.value);
    }
    else
    {
        std::cout << "Syntax Error: expected a identifier or literal \n";
        return nullptr;
    }

    // Comparison
    if (advanceAndCheckEOF())
        return nullptr;
    current_token = getCurrentToken();
    if (current_token.type != TokenType::ComparisonOperator)
    {
        std::cout << "Syntax Error: expected a compare operator \n";
        return nullptr;
    }
    op = current_token.value;

    if (advanceAndCheckEOF())
        return nullptr;
    current_token = getCurrentToken();
    ASTNode *right_child = nullptr;
    if (current_token.type == TokenType::Identifier)
    {
        right_child = new IdentifierNode(current_token.value);
    }
    else if (current_token.type == TokenType::Literal)
    {
        right_child = new LiteralNode(current_token.value);
    }
    else
    {
        delete left_child;
        std::cout << "Syntax Error: Expected identifier or literal for the right-hand side of the condition.\n";
        return nullptr;
    }

    if (advanceAndCheckEOF())
        return nullptr;
    return new ConditionalNode(op, left_child, right_child);
}

/*
    Parse a variable definition
    If we encounter a '=' we parse a assignmnet
*/
ASTNode *Parser::parseVariableDef()
{
    Token current_token = getCurrentToken();
    // Set the data type
    std::string data_type = current_token.value;
    std::string name;
    // Advance to identifier
    if (advanceAndCheckEOF())
        return nullptr;

    current_token = getCurrentToken();
    if (current_token.type != TokenType::Identifier)
    {
        std::cout << "Syntax Error: Expected identifier, got: " << current_token.value << std::endl;
        return nullptr;
    }
    // Set identifier
    name = current_token.value;

    if (advanceAndCheckEOF())
        return nullptr;

    current_token = getCurrentToken();
    if (current_token.value == ";")
    {
        if (advanceAndCheckEOF())
            return nullptr;
        VariableDefNode *var_node = new VariableDefNode(data_type, name);
        defined_variables.insert(name);
        return var_node;
    }
    else if (current_token.value == "=")
    {
        ASTNode *var_node = parseAssignment(new VariableDefNode(data_type, name));
        defined_variables.insert(name);
        return var_node;
    }
    std::cout << "Syntax Error: Expected ';' or '=' after variable declaration" << std::endl;
    return nullptr;
}

/*
    Parse an assignmnet appending the variable definition as its
    its left child and the literal value as the right child
*/
ASTNode *Parser::parseAssignment(ASTNode *left)
{
    // Advance past '='
    if (advanceAndCheckEOF())
        return nullptr;

    Token current_token = getCurrentToken();
    ASTNode *right = parseExpression();

    current_token = getCurrentToken();
    if (current_token.value != ";")
    {
        std::cout << "Expected ';' at end of declaration. Got " << current_token.value << std::endl;
        return nullptr;
    }

    advanceToken();
    current_token = getCurrentToken();
    std::cout << "Current token after parseAssignment: " << current_token.value << std::endl;

    return new AssignmentNode(left, right);
}

ASTNode *Parser::parseIdentifier()
{
    Token current_token = getCurrentToken();
    if (!checkForDefinedVar(current_token.value))
    {
        std::cout << "Syntax Error: Undefined variable " << current_token.value << std::endl;
        return nullptr;
    }
    ASTNode *left = new IdentifierNode(current_token.value);
    // Shift to the "="
    if (advanceAndCheckEOF())
        return nullptr;
    if (getCurrentToken().value != "=")
    {
        std::cout << "Syntax Error: Expected '=' \n";
        return nullptr;
    }
    ASTNode *assignment_node = parseAssignment(left);
    return assignment_node;
}

/*
    Return a parsed expression of arithmic nodes
    Recurisvely call for parseTerm to check for * and /
    Combines terms using addition and subtraction operators.
    In the end, ensures all rigth and left nodes are correctly attached
    preserving operator precendence
*/
ASTNode *Parser::parseExpression()
{

    return nullptr;
}

/*
    Return a literla value or an identifier then advance the index and grab the operator,
    return to parseTerm to check for * and / else return to parseExpression and check for
    + and -
*/
ASTNode *Parser::parseFactor()
{
    Token current_token = getCurrentToken();
    ASTNode *node = nullptr;
    if (current_token.type == TokenType::Identifier)
    {
        node = new IdentifierNode(current_token.value);
    }
    else if (current_token.type == TokenType::Literal)
    {
        node = new LiteralNode(current_token.value);
    }
    advanceToken();
    return node;
}
// Return the current token
Token Parser::getCurrentToken()
{
    return tokens[current_index];
}

// Advance to the next index
void Parser::advanceToken()
{
    current_index++;
}

/*
    Return the next statement to parse
*/
ASTNode *Parser::getNextStatement()
{
    Token current_token = getCurrentToken();
    ASTNode *node = nullptr;
    if (current_token.value == "while")
    {
        node = parseWhileLoop();
    }
    else if (current_token.value == "float" || current_token.value == "int" || current_token.value == "string")
    {
        node = parseVariableDef();
    }
    else if (current_token.value == "if")
    {
        node = parseIfStatement();
    }
    else if (current_token.type == TokenType::Identifier)
    {
        node = parseIdentifier();
    }
    else if (checkForEndOfFile())
    {
        return nullptr;
    }
    else
    {
        return nullptr;
    }
    return node;
}
