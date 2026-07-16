#include "../include/interpreter.h"

double Interpreter::evaluate(const Expr *expr)
{
  if (auto literal = dynamic_cast<const Literal *>(expr))
  {
    return std::stod(literal->value.lexeme);
  }

  if (auto binary = dynamic_cast<const Binary *>(expr))
  {
    double left = evaluate(binary->left.get());

    double right = evaluate(binary->right.get());

    switch (binary->op.type)
    {
    case TokenType::PLUS:

      return left + right;

    default:

      return 0;
    }
  }

  return 0;
}