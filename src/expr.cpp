#include "../include/expr.h"

Literal::Literal(const Token& token)
    : value(token)
{
}

Binary::Binary(std::unique_ptr<Expr> left,
               const Token& op,
               std::unique_ptr<Expr> right)
    : left(std::move(left)),
      op(op),
      right(std::move(right))
{
}

Unary::Unary(const Token& op,
             std::unique_ptr<Expr> right)
    : op(op),
      right(std::move(right))
{
}

Grouping::Grouping(std::unique_ptr<Expr> expression)
    : expression(std::move(expression))
{
}

Variable::Variable(const Token& name)
    : name(name)
{
}

Assignment::Assignment(const Token& name,
                       std::unique_ptr<Expr> value)
    : name(name),
      value(std::move(value))
{
}

IndexExpr::IndexExpr(
    std::unique_ptr<Expr> object,
    std::unique_ptr<Expr> index)
    : object(std::move(object)),
      index(std::move(index))
{
}

IndexAssignment::IndexAssignment(
    std::unique_ptr<Expr> object,
    std::unique_ptr<Expr> index,
    std::unique_ptr<Expr> value)
    : object(std::move(object)),
      index(std::move(index)),
      value(std::move(value))
{
}

NebulaLiteral::NebulaLiteral(
    std::vector<std::unique_ptr<Expr>> elements)
    : elements(std::move(elements))
{
}

VarStmt::VarStmt(const Token& name,
                 std::unique_ptr<Expr> initializer)
    : name(name),
      initializer(std::move(initializer))
{
}

NebulaStmt::NebulaStmt(const Token& name,
                       std::unique_ptr<Expr> initializer)
    : name(name),
      initializer(std::move(initializer))
{
}

PrintStmt::PrintStmt(std::unique_ptr<Expr> expression)
    : expression(std::move(expression))
{
}

ExpressionStmt::ExpressionStmt(std::unique_ptr<Expr> expression)
    : expression(std::move(expression))
{
}

InputStmt::InputStmt(const Token& name)
    : name(name)
{
}

IfStmt::IfStmt(
    std::unique_ptr<Expr> condition,
    std::vector<std::unique_ptr<Stmt>> thenBranch,
    std::vector<std::unique_ptr<Stmt>> elseBranch)
    : condition(std::move(condition)),
      thenBranch(std::move(thenBranch)),
      elseBranch(std::move(elseBranch))
{
}

WhileStmt::WhileStmt(
    std::unique_ptr<Expr> condition,
    std::vector<std::unique_ptr<Stmt>> body)
    : condition(std::move(condition)),
      body(std::move(body))
{
}