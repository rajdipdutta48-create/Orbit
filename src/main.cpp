#include <iostream>

#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"

int main()
{
    std::string source = R"(

dock age = 20;

transmit(age);

transmit(20 + 30 * 5);

)";

    // Step 1: Convert source code into tokens
    Lexer lexer(source);

    std::vector<Token> tokens = lexer.scanTokens();

    // Step 2: Convert tokens into statements / AST
    Parser parser(tokens);

    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    // Step 3: Execute the Orbit program
    Interpreter interpreter;

    interpreter.interpret(statements);

    return 0;
}