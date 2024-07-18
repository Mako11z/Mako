#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <stack>
#include "parser.h"
#include "codegen.h"

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
            // Add logical operator
            node->logicalOps.push_back(current_token.value);
            // Advance
            if (advanceAndCheckEOF())
            {
                delete node;
                return nullptr;
            }
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
    {
        delete node;
        return nullptr;
    }

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
    {
        return nullptr;
    }

    BodyNode *body_node = getBodyStatements();
    if (body_node == nullptr)
    {
        delete if_node;
        return nullptr;
    }
    current_token = getCurrentToken();
    if (current_token.value != "}")
    {
        std::cout << "Expected '}' \n";
        delete if_node;
        delete body_node;
        return nullptr;
    }
    if (advanceAndCheckEOF())
    {
        delete if_node;
        delete body_node;
        return nullptr;
    }

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
    if (body_node == nullptr)
    {
        delete body_node;
        return nullptr;
    }
    current_token = getCurrentToken();
    if (current_token.value != "}")
    {
        std::cout << "Expected '}' \n";
        delete body_node;
        delete while_node;
        return nullptr;
    }
    if (advanceAndCheckEOF())
    {
        delete while_node;
        return nullptr;
        return nullptr;
    }

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
    {
        delete left_child;
        return nullptr;
    }

    current_token = getCurrentToken();
    if (current_token.type != TokenType::ComparisonOperator)
    {
        std::cout << "Syntax Error: expected a compare operator \n";
        delete left_child;
        return nullptr;
    }
    op = current_token.value;

    if (advanceAndCheckEOF())
    {
        delete left_child;
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

    if (advanceAndCheckEOF())
    {
        delete left_child;
        delete right_child;
        return nullptr;
    }

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

    ASTNode *right = parseExpression();

    Token current_token = getCurrentToken();
    if (current_token.value != ";")
    {
        std::cout << "Expected ';' at end of declaration. Got " << current_token.value << std::endl;
        delete right;
        return nullptr;
    }

    advanceToken();
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
    {
        delete left;
        return nullptr;
    }

    if (getCurrentToken().value != "=")
    {
        std::cout << "Syntax Error: Expected '=' \n";
        delete left;
        return nullptr;
    }
    ASTNode *assignment_node = parseAssignment(left);
    return assignment_node;
}

/*
    Add all operands to deque
    Add operators to another deque
    For the first pass, take care of the * and / operations
    For the second pass, take care of all of the + and - operations
*/
ASTNode *Parser::parseExpression()
{
    std::deque<ASTNode *> operands;
    std::deque<std::string> ops;
    // Do all multiplication and division then add arithmic nodes to deque
    while (!checkForEndOfFile())
    {
        Token current_token = getCurrentToken();
        if (current_token.type == TokenType::Literal || current_token.type == TokenType::Identifier)
        {
            ASTNode *factor = parseFactor();
            if (!factor)
            {
                // Cleanup operands if parseFactor fails
                for (auto *operand : operands)
                {
                    delete operand;
                }
                return nullptr;
            }
            operands.push_back(factor);
            advanceToken();
        }
        else if (current_token.value == "*" || current_token.value == "/")
        {
            std::string val = current_token.value;
            // Grab the next operand
            advanceToken();
            ASTNode *right = parseFactor();
            if (!right)
            {
                for (auto *operand : operands)
                {
                    delete operand;
                }
                return nullptr;
            }
            ASTNode *left = operands.back();
            // Pop from the deque and replace with new node
            operands.pop_back();
            operands.push_back(new ArithmicNode(val, left, right));
            advanceToken();
        }
        else if (current_token.value == "+" || current_token.value == "-")
        {
            ops.push_back(current_token.value);
            advanceToken();
        }
        else if (current_token.value == ";")
        {
            break;
        }
        else
        {
            std::cout << "Unexpected token: " << current_token.value << std::endl;
            for (auto *operand : operands)
            {
                delete operand;
            }
            return nullptr;
        }
    }
    // Do all addition and subtraction and add arithmic nodes to deque
    while (!ops.empty())
    {
        ASTNode *left = operands.front();
        operands.pop_front();
        ASTNode *right = operands.front();
        operands.pop_front();
        std::string val = ops.front();
        ops.pop_front();
        operands.push_front(new ArithmicNode(val, left, right));
    }

    return operands.front();
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
    else
    {
        std::cout << "Expected an identifier or literal, got: " << current_token.value << std::endl;
        delete node;
        return nullptr;
    }
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

void Parser::printAST(ASTNode *node, int indent)
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
    {
        AssignmentNode *assign = dynamic_cast<AssignmentNode *>(node);
        std::cout << indentStr << "AssignmentNode\n";
        printAST(assign->children[0], indent + 2); // left child
        printAST(assign->children[1], indent + 2); // right child
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
        std::cout << indentStr << "ConditionalNode(operation: " << condNode->operation << ")\n";
        std::cout << indentStr << "  Left:\n";
        printAST(condNode->children[0], indent + 4);
        std::cout << indentStr << "  Right:\n";
        printAST(condNode->children[1], indent + 4);
    }
    break;

    case ASTNodeType::ArithmicExpressions:
    {
        ArithmicNode *arithNode = dynamic_cast<ArithmicNode *>(node);
        std::cout << indentStr << "ArithmicNode(operation: " << arithNode->op << ")\n";
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
        std::cout << indentStr << "IdentifierNode(name: " << idNode->name;
        std::cout << ")\n";
    }
    break;

    default:
        std::cout << indentStr << "Unknown ASTNode type\n";
        break;
    }
}