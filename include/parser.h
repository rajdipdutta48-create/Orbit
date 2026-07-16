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

public:
  Parser(const std::vector<Token> &tokens);
};

#endif