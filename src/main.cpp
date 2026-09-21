#include <iostream>
#include <fstream>
#include <sstream>

#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"

std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TokenType::DOCK: return "DOCK";
    case TokenType::TRANSMIT: return "TRANSMIT";
    case TokenType::RECEIVE: return "RECEIVE";
    case TokenType::TRUE: return "TRUE";
    case TokenType::FALSE: return "FALSE";
    case TokenType::WHEN: return "WHEN";
    case TokenType::ELSE: return "ELSE";
    case TokenType::ORBITING: return "ORBITING";
    case TokenType::NEBULA: return "NEBULA";

    case TokenType::IDENTIFIER: return "IDENTIFIER";
    case TokenType::NUMBER: return "NUMBER";
    case TokenType::STRING: return "STRING";
    case TokenType::CHAR: return "CHAR";

    case TokenType::PLUS: return "PLUS";
    case TokenType::MINUS: return "MINUS";
    case TokenType::STAR: return "STAR";
    case TokenType::SLASH: return "SLASH";
    case TokenType::EQUAL: return "EQUAL";
    case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
    case TokenType::NOT_EQUAL: return "NOT_EQUAL";
    case TokenType::LESS: return "LESS";
    case TokenType::LESS_EQUAL: return "LESS_EQUAL";
    case TokenType::GREATER: return "GREATER";
    case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
    case TokenType::BANG: return "BANG";
    case TokenType::AND: return "AND";
    case TokenType::OR: return "OR";

    case TokenType::LEFT_PAREN: return "LEFT_PAREN";
    case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
    case TokenType::LEFT_BRACE: return "LEFT_BRACE";
    case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
    case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
    case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
    case TokenType::COMMA: return "COMMA";
    case TokenType::SEMICOLON: return "SEMICOLON";

    case TokenType::END_OF_FILE: return "END_OF_FILE";
    }

    return "UNKNOWN";
}

int main(int argc, char* argv[])
{
    std::string source;

    /*
     * Token viewer mode.
     *
     * Example:
     * ./orbit --tokens program.orbit
     *
     * This runs only the lexer and prints every token.
     */
    if (argc >= 3 && std::string(argv[1]) == "--tokens")
    {
        std::ifstream file(argv[2]);

        if (!file)
        {
            std::cerr << "Could not open Orbit source file: "
                      << argv[2]
                      << std::endl;

            return 1;
        }

        std::stringstream buffer;

        buffer << file.rdbuf();

        source = buffer.str();

        Lexer lexer(source);

        std::vector<Token> tokens = lexer.scanTokens();

        for (const Token& token : tokens)
        {
            std::cout
                << tokenTypeToString(token.type)
                << " | "
                << token.lexeme
                << " | line "
                << token.line
                << std::endl;
        }

        return 0;
    }

    // When a source file is provided, load the Orbit program
    // from that file.
    //
    // Example:
    // ./orbit program.orbit
    if (argc >= 2)
    {
        std::ifstream file(argv[1]);

        if (!file)
        {
            std::cerr << "Could not open Orbit source file: "
                      << argv[1]
                      << std::endl;

            return 1;
        }

        std::stringstream buffer;

        buffer << file.rdbuf();

        source = buffer.str();
    }
    else
    {
        // Default program used for testing String + Char
        // input without needing a separate Orbit file.
        source = R"(
dock age = 0;
dock name = "Default";
dock letter = 'X';

nebula names = ["Earth", "Mars"];
nebula letters = ['A', 'B'];

receive(age);
receive(name);
receive(letter);

receive(names[0]);
receive(letters[1]);

transmit(age);
transmit(name);
transmit(letter);
transmit(names[0]);
transmit(letters[1]);
)";
    }

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