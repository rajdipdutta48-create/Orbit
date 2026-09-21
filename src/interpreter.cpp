#include "../include/interpreter.h"

double Interpreter::evaluate(const Expr *expr)
{
    // Evaluate a number literal.
    if (auto literal = dynamic_cast<const Literal *>(expr))
    {
        return std::stod(literal->value.lexeme);
    }

    // Evaluate a binary expression recursively.
    if (auto binary = dynamic_cast<const Binary *>(expr))
    {
        double left = evaluate(binary->left.get());
        double right = evaluate(binary->right.get());

        switch (binary->op.type)
        {
        case TokenType::PLUS:
            return left + right;

        case TokenType::MINUS:
            return left - right;

        case TokenType::STAR:
            return left * right;

        case TokenType::SLASH:
            return left / right;

        default:
            return 0;
        }
    }

    // Evaluate a parenthesized expression.
    if (auto grouping = dynamic_cast<const Grouping *>(expr))
    {
        return evaluate(grouping->expression.get());
    }

    return 0;
}