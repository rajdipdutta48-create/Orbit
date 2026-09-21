#ifndef EXPR_H
#define EXPR_H

#include <memory>
#include <vector>
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

// Represents unary expressions such as:
// -10
// -age
class Unary : public Expr
{
public:
    Token op;
    std::unique_ptr<Expr> right;

    Unary(const Token& op,
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

// Represents:
// age = 25;
class Assignment : public Expr
{
public:
    Token name;
    std::unique_ptr<Expr> value;

    Assignment(const Token& name,
               std::unique_ptr<Expr> value);
};

// Represents:
// numbers[0]
class IndexExpr : public Expr
{
public:
    std::unique_ptr<Expr> object;
    std::unique_ptr<Expr> index;

    IndexExpr(
        std::unique_ptr<Expr> object,
        std::unique_ptr<Expr> index);
};

// Represents:
// numbers[0] = 99
class IndexAssignment : public Expr
{
public:
    std::unique_ptr<Expr> object;
    std::unique_ptr<Expr> index;
    std::unique_ptr<Expr> value;

    IndexAssignment(
        std::unique_ptr<Expr> object,
        std::unique_ptr<Expr> index,
        std::unique_ptr<Expr> value);
};

// Represents:
// [10, 20, 30]
class NebulaLiteral : public Expr
{
public:
    std::vector<std::unique_ptr<Expr>> elements;

    NebulaLiteral(
        std::vector<std::unique_ptr<Expr>> elements);
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
// nebula numbers = [10, 20, 30];
class NebulaStmt : public Stmt
{
public:
    Token name;
    std::unique_ptr<Expr> initializer;

    NebulaStmt(const Token& name,
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

// Represents:
//
// when (condition) {
//     statements
// } else {
//     statements
// }
class IfStmt : public Stmt
{
public:
    std::unique_ptr<Expr> condition;

    std::vector<std::unique_ptr<Stmt>> thenBranch;
    std::vector<std::unique_ptr<Stmt>> elseBranch;

    IfStmt(
        std::unique_ptr<Expr> condition,
        std::vector<std::unique_ptr<Stmt>> thenBranch,
        std::vector<std::unique_ptr<Stmt>> elseBranch);
};

// Represents:
//
// orbiting (condition) {
//     statements
// }
class WhileStmt : public Stmt
{
public:
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> body;

    WhileStmt(
        std::unique_ptr<Expr> condition,
        std::vector<std::unique_ptr<Stmt>> body);
};

#endif