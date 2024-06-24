#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <set>

enum class TokenType
{
    keyword,
    Identifier,
    Literal,
    Operator,
    Punctuation,
    ComparisonOperator,
    AssignmentOperator,
};

struct Token
{
    TokenType type;
    std::string value;
};

class Lexer
{
public:
    Lexer(std::string &fileName)
    {
        this->fileName = fileName;
    }

    std::vector<Token> tokenize()
    {
        std::ifstream file(fileName);
        std::vector<Token> tokens;

        if (!file.is_open())
        {
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

private:
    std::string fileName, buffer;
    std::set<std::string> keywords = {"int", "float", "string", "char", "if", "else", "for", "while"};
};

int main()
{
    std::string name = "test.txt";
    Lexer lexer(name);
    std::vector<Token> tokens = lexer.tokenize();
    for (const Token &token : tokens)
    {
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
}