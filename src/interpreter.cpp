#include "../include/interpreter.h"
#include <variant>

Value Interpreter::evaluate(const Expr* expr)
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
        Value right = evaluate(unary->right.get());

        if (unary->op.type == TokenType::MINUS)
        {
            return -std::get<double>(right);
        }

        return 0.0;
    }

    // Evaluate a binary expression recursively.
    if (auto binary = dynamic_cast<const Binary*>(expr))
    {
        Value left = evaluate(binary->left.get());
        Value right = evaluate(binary->right.get());

        // Arithmetic operators work with numbers.
        switch (binary->op.type)
        {
        case TokenType::PLUS:
            return std::get<double>(left) + std::get<double>(right);

        case TokenType::MINUS:
            return std::get<double>(left) - std::get<double>(right);

        case TokenType::STAR:
            return std::get<double>(left) * std::get<double>(right);

        case TokenType::SLASH:
            return std::get<double>(left) / std::get<double>(right);

        // Comparison operators produce boolean values.
        case TokenType::LESS:
            return std::get<double>(left) < std::get<double>(right);

        case TokenType::LESS_EQUAL:
            return std::get<double>(left) <= std::get<double>(right);

        case TokenType::GREATER:
            return std::get<double>(left) > std::get<double>(right);

        case TokenType::GREATER_EQUAL:
            return std::get<double>(left) >= std::get<double>(right);

        case TokenType::EQUAL_EQUAL:
            return left == right;

        case TokenType::NOT_EQUAL:
            return left != right;

        default:
            return 0.0;
        }
    }

    // Evaluate a parenthesized expression.
    if (auto grouping = dynamic_cast<const Grouping*>(expr))
    {
        return evaluate(grouping->expression.get());
    }

    return 0.0;
}

void Interpreter::printValue(const Value& value)
{
    // Print numbers.
    if (std::holds_alternative<double>(value))
    {
        std::cout << std::get<double>(value);
    }

    // Print booleans.
    else if (std::holds_alternative<bool>(value))
    {
        std::cout << (std::get<bool>(value) ? "true" : "false");
    }

    std::cout << std::endl;
}

void Interpreter::execute(const Stmt* stmt)
{
    // Variable declaration:
    // dock age = 20;
    if (auto var = dynamic_cast<const VarStmt*>(stmt))
    {
        Value value = evaluate(var->initializer.get());

        environment[var->name.lexeme] = value;

        return;
    }

    // Print statement:
    // transmit(age);
    if (auto print = dynamic_cast<const PrintStmt*>(stmt))
    {
        Value value = evaluate(print->expression.get());

        printValue(value);

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