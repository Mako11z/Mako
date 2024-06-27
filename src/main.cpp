#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "parser.h"
#include "lexer.h"

int main()
{
    std::string file_name = "test.txt";
    Lexer lexer(file_name);

    std::vector<Token> tokens = lexer.tokenize();
    for (const Token &token : tokens)
    {
        // Print token type
        switch (token.type)
        {
        case TokenType::keyword:
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

    return 0;
}