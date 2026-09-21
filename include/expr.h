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

// Base class for all Orbit statements.
class Stmt
{
public:
    virtual ~Stmt() = default;
};

// Represents:
// dock age = 20;
class VarStmt : public Stmt
{
public:
    Token name;
    std::unique_ptr<Expr> initializer;

    VarStmt(const Token& name,
            std::unique_ptr<Expr> initializer);
};

// Represents:
// transmit(age);
class PrintStmt : public Stmt
{
public:
    std::unique_ptr<Expr> expression;

    PrintStmt(std::unique_ptr<Expr> expression);
};

// Represents a standalone expression:
// 20 + 30;
class ExpressionStmt : public Stmt
{
public:
    std::unique_ptr<Expr> expression;

    ExpressionStmt(std::unique_ptr<Expr> expression);
};

// Represents:
// receive(age);
class InputStmt : public Stmt
{
public:
    Token name;

    InputStmt(const Token& name);
};

#endif