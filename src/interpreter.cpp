#include "../include/interpreter.h"

double Interpreter::evaluate(const Expr* expr)
{
    // Evaluate a number literal.
    if (auto literal = dynamic_cast<const Literal*>(expr))
    {
        return std::stod(literal->value.lexeme);
    }

    // Evaluate a variable.
    if (auto variable = dynamic_cast<const Variable*>(expr))
    {
        return environment[variable->name.lexeme];
    }

    // Evaluate a unary expression.
    // Example:
    // -10
    // -age
    if (auto unary = dynamic_cast<const Unary*>(expr))
    {
        double right = evaluate(unary->right.get());

        switch (unary->op.type)
        {
        case TokenType::MINUS:
            return -right;

        default:
            return 0;
        }
    }

    // Evaluate a binary expression recursively.
    if (auto binary = dynamic_cast<const Binary*>(expr))
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
    if (auto grouping = dynamic_cast<const Grouping*>(expr))
    {
        return evaluate(grouping->expression.get());
    }

    return 0;
}

void Interpreter::execute(const Stmt* stmt)
{
    // Variable declaration:
    // dock age = 20;
    if (auto var = dynamic_cast<const VarStmt*>(stmt))
    {
        double value = evaluate(var->initializer.get());

        environment[var->name.lexeme] = value;

        return;
    }

    // Print statement:
    // transmit(age);
    if (auto print = dynamic_cast<const PrintStmt*>(stmt))
    {
        std::cout << evaluate(print->expression.get())
                  << std::endl;

        return;
    }

    // Input statement:
    // receive(age);
    if (auto input = dynamic_cast<const InputStmt*>(stmt))
    {
        double value;

        std::cout << "Enter " << input->name.lexeme << ": ";

        std::cin >> value;

        environment[input->name.lexeme] = value;

        return;
    }

    // Standalone expression:
    // 20 + 30;
    if (auto expressionStmt = dynamic_cast<const ExpressionStmt*>(stmt))
    {
        evaluate(expressionStmt->expression.get());

        return;
    }
}

void Interpreter::interpret(
    const std::vector<std::unique_ptr<Stmt>>& statements)
{
    // Execute every statement in the Orbit program in order.
    for (const auto& statement : statements)
    {
        execute(statement.get());
    }
}