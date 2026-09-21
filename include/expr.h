#ifndef EXPR_H
#define EXPR_H

#include <memory>
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

    Literal(const Token& value);
};

class Binary : public Expr
{
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left,
           const Token& op,
           std::unique_ptr<Expr> right);
};

class Grouping : public Expr
{
public:
    std::unique_ptr<Expr> expression;

    Grouping(std::unique_ptr<Expr> expression);
};

class Variable : public Expr
{
public:
    Token name;

    Variable(const Token& name);
};

#endif