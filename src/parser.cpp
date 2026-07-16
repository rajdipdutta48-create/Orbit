#include "../include/parser.h"

Parser::Parser(const std::vector<Token> &tokens)
{
  this->tokens = tokens;
  current = 0;
}