#ifndef EXPR_H
#define EXPR_H

#include "token.h"

class Expr
{
public:
  virtual ~Expr() = default;
};

class Literal : public Expr
{
public:
  Token value;

  Literal(const Token &value);
};

#endif