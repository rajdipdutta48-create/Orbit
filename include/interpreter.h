#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <memory>

#include "expr.h"

class Interpreter
{
public:
  double evaluate(const Expr *expr);
};

#endif