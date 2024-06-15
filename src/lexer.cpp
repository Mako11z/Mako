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

enum class LexerState
{
    Initial,
    Identifier,
    Number,
    Operator,
    Punctuation,
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
        state = LexerState::Initial;
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
            if (state == LexerState::Initial)
            {
                if (isspace(current))
                {
                    continue;
                }
                else if (isalpha(current))
                {
                    buffer = current;
                    state = LexerState::Identifier;
                }
                else if (isdigit(current))
                {
                    buffer = current;
                    state = LexerState::Number;
                }
                else if (current == '*' || current == '-' || current == '+' || current == '/' || current == '%')
                {
                    tokens.push_back({TokenType::Operator, std::string(1, current)});
                }
                else if (current == '=')
                {
                    handleAssignmentOperator(current, tokens, file);
                    // tokens.push_back({ TokenType::AssignmentOperator, std::string(1, current) });
                }
                else if (current == '>' || current == '<' || current == '!')
                {
                    handleComparisonOperator(current, tokens, file);
                    // tokens.push_back({ TokenType::ComparisonOperator, std::string(1, current) });
                }
                else if (current == '(' || current == ')' || current == '{' || current == '}' || current == ';')
                {
                    tokens.push_back({TokenType::Punctuation, std::string(1, current)});
                }
                else
                {
                    state = LexerState::Initial;
                }
            }

            else if (state == LexerState::Identifier)
            {
                if (isalnum(current) || current == '_')
                {
                    buffer += current;
                }
                else
                {
                    if (keywords.find(buffer) != keywords.end())
                    {
                        tokens.push_back({TokenType::keyword, buffer});
                    }
                    else
                    {
                        tokens.push_back({TokenType::Identifier, buffer});
                    }
                    buffer.clear();
                    state = LexerState::Initial;
                    // Put that character back into the stream to be processed
                    file.unget();
                }
            }

            else if (state == LexerState::Number)
            {
                if (isdigit(current))
                {
                    buffer += current;
                }
                else
                {
                    tokens.push_back({TokenType::Literal, buffer});
                    buffer.clear();
                    state = LexerState::Initial;
                    // Put that character back into the stream to be processed
                    file.unget();
                }
            }
        }
        if (!buffer.empty())
        {
            if (state == LexerState::Identifier)
            {
                if (keywords.find(buffer) != keywords.end())
                {
                    tokens.push_back({TokenType::keyword, buffer});
                }
                else
                {
                    tokens.push_back({TokenType::Identifier, buffer});
                }
            }
            else if (state == LexerState::Number)
            {
                tokens.push_back({TokenType::Literal, buffer});
            }
        }
        file.close();
        return tokens;
    }

private:
    LexerState state;
    std::string fileName, buffer;
    std::set<std::string> keywords = {"int", "float", "string", "char", "if", "else", "for", "while"};
    void handleAssignmentOperator(char current, std::vector<Token> &tokens, std::ifstream &file)
    {
        char next;
        // Check if there a next character
        if (file.get(next))
        {
            // Check for '==' conditional
            if (next == '=')
            {
                tokens.push_back({TokenType::ComparisonOperator, "=="});
            }
            else
            {
                // We did not find it was a conditional so its a assignment operator '='
                tokens.push_back({TokenType::AssignmentOperator, std::string(1, current)});
                // Reprocess the character
                file.unget();
            }
        }
        else
        {
            tokens.push_back({TokenType::AssignmentOperator, std::string(1, current)});
        }
    }
    void handleComparisonOperator(char current, std::vector<Token> &tokens, std::ifstream &file)
    {
        char next;
        if (file.get(next))
        {
            if (next == '=')
            {
                tokens.push_back({TokenType::ComparisonOperator, std::string(1, current) + next});
            }
            else
            {
                tokens.push_back({TokenType::ComparisonOperator, std::string(1, current)});
                file.unget();
            }
        }
        else
        {
            tokens.push_back({TokenType::ComparisonOperator, std::string(1, current)});
        }
    }
};

int main()
{
    std::string name = "test.txt";
    Lexer lexer(name);
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
        // Print token value
        std::cout << token.value << std::endl;
    }
}