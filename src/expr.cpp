#include "../include/expr.h"
Literal::Literal(const Token &token)
    : value(token)
{
}

Binary::Binary(std::unique_ptr<Expr> left,
               const Token &op,
               std::unique_ptr<Expr> right)
    : left(std::move(left)),
      op(op),
      right(std::move(right))
{
}