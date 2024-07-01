#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <set>

enum class TokenType
{
    Keyword,
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
    Lexer(std::string &fileName) : fileName(fileName), keywords{"int", "float", "string", "char", "if", "else", "for", "while"} {}
    std::vector<Token> tokenize();

private:
    std::string fileName, buffer;
    std::set<std::string> keywords = {"int", "float", "string", "char", "if", "else", "for", "while"};
};

#endif