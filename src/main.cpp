#include <iostream>

#include "../include/lexer.h"

int main()
{
  std::string source = R"(

comet:

This is Orbit.

burn

let age = 20;

print("Hello");

)";

  Lexer lexer(source);

  std::vector<Token> tokens = lexer.scanTokens();

  for (const Token &token : tokens)
  {
    std::cout
        << token.lexeme
        << "  "
        << token.line
        << std::endl;
  }

  return 0;
}