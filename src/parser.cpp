#include <iostream>
#include <string>
#include <vector>
#include "parser.h"

ASTNode *Parser::parse()
{
    ProgramNode *program = new ProgramNode();
    ASTNode *curr_node = NULL;
    curr_node = parseVariableDef();
    program->children.push_back(curr_node);
    /*
    while (current_index < tokens.size())
    {
        Token current_token = getCurrentToken();
        ASTNode *curr_node = NULL;
        if (current_token.type == TokenType::Keyword)
        {
            if (current_token.value == "float" || current_token.value == "int" || current_token.value == "string" || current_token.value == "char")
            {
                curr_node = parseVariableDef();
            }
            else if (current_token.value == "while")
            {
                curr_node = parseWhileLoop();
            }
            else if (current_token.value == "if")
            {
                curr_node = parseIfStatement();
            }
        }
        else if (current_token.type == TokenType::Identifier)
        {
            curr_node = parseLiteral();
        }

        if (curr_node)
        {
            program->children.push_back(curr_node);
        }
        // We have reached the end of a statement
        advanceToken();
    }
    */
    return program;
}

bool Parser::checkForEndOfFile()
{
    return current_index > tokens.size();
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
    current_token = getCurrentToken();
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
ASTNode *Parser::parseWhileLoop()
{

    return NULL;
}
ASTNode *Parser::parseIfStatement()
{

    return NULL;
}
ASTNode *Parser::parseLiteral()
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