#include <iostream>

#include "../include/lexer.h"

int main()
{
  std::string source;
  
  Lexer lexer(source);

  std::vector<Token> tokens = lexer.scanTokens();

  return 0;
}