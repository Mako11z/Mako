#include <iostream>
#include <string>
#include <vector>
#include "parser.h"

ASTNode *Parser::parse()
{
    ProgramNode *program = new ProgramNode();
    ASTNode *curr_node = NULL;
    curr_node = parseWhileLoop();
    program->children.push_back(curr_node);

    return program;
}

bool Parser::checkForEndOfFile()
{
    return current_index > tokens.size();
}

bool Parser::checkForDefinedVar(const std::string &name)
{
    if (defined_variables.find(name) != defined_variables.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

ASTNode *Parser::parseWhileLoop()
{
    // Skip the 'while' keyword and check for nullptr
    advanceToken();
    if (checkForEndOfFile())
    {
        std::cout << "End of File after while keyword, expected '(' " << std::endl;
        return nullptr;
    }
    Token current_token = getCurrentToken();
    if (current_token.value != "(")
    {
        std::cout << "Syntax Error, Expected '(' " << std::endl;
        return nullptr;
    }
    advanceToken();

    WhileLoopNode *while_node = new WhileLoopNode();
    // Loop until end of condition
    current_token = getCurrentToken();
    while (current_token.value != ")")
    {
        if (checkForEndOfFile())
        {
            std::cout << "End of file while parsing while loop" << std::endl;
            delete while_node;
            return nullptr;
        }
        ASTNode *condition = parseCondition();
        if (condition == nullptr)
        {
            std::cout << "Error parsing condition in while loop \n";
            delete while_node;
            return nullptr;
        }
        while_node->conditions.push_back(condition);

        current_token = getCurrentToken();
        if (current_token.value == ")")
        {
            break;
        }
        else if (current_token.type == TokenType::LogicalOperator)
        {
            advanceToken();
        }
        else
        {
            delete while_node;
            std::cout << "Expected a logical operator or ') \n";
            return nullptr;
        }
        current_token = getCurrentToken();
    }

    return while_node;
}

// Parse a condition and grab the left operand, operator, and rigth operand
ASTNode *Parser::parseCondition()
{
    Token current_token = getCurrentToken();
    std::string op;
    ASTNode *left_child = nullptr;

    // Left child
    if (current_token.type == TokenType::Identifier)
    {
        std::cout << current_token.value << std::endl;
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
    advanceToken();
    if (checkForEndOfFile())
    {
        std::cout << "Syntax Error: end of file, expected a compare operator \n";
        delete left_child;
        return nullptr;
    }
    current_token = getCurrentToken();
    if (current_token.type != TokenType::ComparisonOperator)
    {
        std::cout << "Syntax Error: expected a compare operator \n";
        return nullptr;
    }
    std::cout << current_token.value << std::endl;
    op = current_token.value;
    std::cout << op << std::endl;
    advanceToken();
    if (checkForEndOfFile())
    {
        delete left_child;
        std::cout << "End of File after operator, expected right-hand side of condition\n";
        return nullptr;
    }
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
    advanceToken();
    return new ConditionalNode(op, left_child, right_child);
    ;
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

    advanceToken();
    if (checkForEndOfFile())
    {
        std::cout << "End of File after data type" << std::endl;
        return nullptr;
    }
    current_token = getCurrentToken();
    if (current_token.type != TokenType::Identifier)
    {
        std::cout << "Syntax Error: Expected identifier, got: " << current_token.value << std::endl;
        return nullptr;
    }
    // Set identifier
    name = current_token.value;
    // Add to set of variables we have defined
    defined_variables.insert(name);

    advanceToken();
    if (checkForEndOfFile())
    {
        std::cout << "End of File after identifier" << std::endl;
        return nullptr;
    }

    current_token = getCurrentToken();
    if (current_token.value == ";")
    {
        advanceToken();
        return new VariableDefNode(data_type, name);
    }
    else if (current_token.value == "=")
    {
        return parseAssignment(new VariableDefNode(data_type, name));
    }
    std::cout << "Syntax Error: Expected ';' or '=' after variable declaration" << std::endl;
    return nullptr;
}

ASTNode *Parser::parseIfStatement()
{

    return NULL;
}

/*
    Parse an assignmnet appending the variable definition as its
    its left child and the literal value as the right child
*/
ASTNode *Parser::parseAssignment(VariableDefNode *left)
{
    // Get the right child
    advanceToken();
    if (checkForEndOfFile())
    {
        std::cout << "End of File" << std::endl;
        return NULL;
    }

    Token current_token = getCurrentToken();
    if (current_token.type != TokenType::Literal)
    {
        std::cout << "Expected a literal" << std::endl;
        return nullptr;
    }
    // Grab the literal value
    LiteralNode *literal = new LiteralNode(current_token.value);
    advanceToken();
    current_token = getCurrentToken();
    if (current_token.value != ";")
    {
        std::cout << "Expected ';' at end of declaration" << std::endl;
        return nullptr;
    }
    return new AssignmentNode(left, literal);
}

// Return the current token
Token Parser::getCurrentToken()
{
    return tokens[current_index];
}

// Peek ahead to the next token
Token Parser::peekToken()
{
    if (current_index + 1 < tokens.size())
    {
        return tokens[current_index + 1];
    }
    else
    {
        return Token{TokenType::Punctuation, "End of File"};
    }
}

// Advance to the next index
void Parser::advanceToken()
{
    current_index++;
}