#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>

#include "token.h"
#include "expr.h"

class Parser
{
private:
  std::vector<Token> tokens;

  int current;

  bool isAtEnd();

  Token peek();

  Token previous();

  Token advance();

  bool check(TokenType type);

  bool match(TokenType type);

public:
  Parser(const std::vector<Token> &tokens);
};

#endif