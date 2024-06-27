#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <set>
#include "lexer.h"

std::vector<Token> Lexer::tokenize()
{
    std::ifstream file(fileName);
    std::vector<Token> tokens;

    if (!file.is_open())
    {
        std::cerr << "File did not open " << fileName << std::endl;
        return tokens;
    }

    char current;
    while (file.get(current))
    {
        if (isspace(current))
        {
            continue;
        }
        else if (isalpha(current) || current == '_')
        {
            buffer = current;
            char next = file.peek();
            while (isalpha(next) || isdigit(next) || next == '_')
            {
                file.get(current);
                buffer += current;
                next = file.peek();
            }
            if (keywords.find(buffer) != keywords.end())
            {
                tokens.push_back({TokenType::keyword, buffer});
            }
            else
            {
                tokens.push_back({TokenType::Identifier, buffer});
            }
            buffer.clear();
        }
        else if (isdigit(current))
        {
            buffer = current;
            char next = file.peek();
            while (isdigit(next))
            {
                file.get(current);
                buffer += current;
                next = file.peek();
            }
            tokens.push_back({TokenType::Literal, buffer});
            buffer.clear();
        }
        else if (current == '*' || current == '-' || current == '+' || current == '/' || current == '%')
        {
            tokens.push_back({TokenType::Operator, std::string(1, current)});
        }
        else if (current == '=')
        {
            char next = file.peek();
            if (next == '=')
            {
                file.get();
                tokens.push_back({TokenType::ComparisonOperator, "=="});
            }
            else
            {
                tokens.push_back({TokenType::AssignmentOperator, "="});
            }
        }
        else if (current == '>' || current == '<' || current == '!')
        {
            char next = file.peek();
            if (next == '=')
            {
                tokens.push_back({TokenType::ComparisonOperator, std::string(1, current) + '='});
                file.get();
            }
            else
            {
                tokens.push_back({TokenType::ComparisonOperator, std::string(1, current)});
            }
        }
        else if (current == '(' || current == ')' || current == '{' || current == '}' || current == ';')
        {
            tokens.push_back({TokenType::Punctuation, std::string(1, current)});
        }
    }

    file.close();
    return tokens;
}
