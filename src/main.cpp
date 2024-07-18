#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "parser.h"
#include "lexer.h"
#include "codegen.h"

int main()
{
    std::string file_name = "test.txt";
    Lexer lexer(file_name);

    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);

    ASTNode *root = parser.parse();
    if (!root)
    {
        std::cerr << "Error parsing the program.\n";
        return 1;
    }
    parser.printAST(root, 0);

    CodeGenerator codegen;
    codegen.generateCode(root);
    codegen.printIR();
    codegen.printIR("output.ll");

    return 0;
}