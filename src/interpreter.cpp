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
        auto it = environment.find(variable->name.lexeme);

        // Variable does not exist.
        if (it == environment.end())
        {
            throw RuntimeError(
                "Undefined variable '" +
                variable->name.lexeme +
                "'");
        }

        return it->second;
    }

    // Evaluate an assignment.
    // Example:
    // age = 25;
    if (auto assignment = dynamic_cast<const Assignment*>(expr))
    {
        // Assignment is only valid for an existing variable.
        auto it = environment.find(assignment->name.lexeme);

        if (it == environment.end())
        {
            throw RuntimeError(
                "Undefined variable '" +
                assignment->name.lexeme +
                "'");
        }

        Value value = evaluate(assignment->value.get());

        it->second = value;

        return value;
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

        throw RuntimeError("Unknown unary operator");
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
            if (std::get<double>(right) == 0)
            {
                throw RuntimeError("Division by zero");
            }

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
            throw RuntimeError("Unknown binary operator");
        }
    }

    // Evaluate a parenthesized expression.
    if (auto grouping = dynamic_cast<const Grouping*>(expr))
    {
        return evaluate(grouping->expression.get());
    }

    throw RuntimeError("Unknown expression");
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
    // age = 25;
    if (auto expressionStmt = dynamic_cast<const ExpressionStmt*>(stmt))
    {
        evaluate(expressionStmt->expression.get());

        return;
    }
}

void Interpreter::interpret(
    const std::vector<std::unique_ptr<Stmt>>& statements)
{
    try
    {
        // Execute every statement in the Orbit program in order.
        for (const auto& statement : statements)
        {
            execute(statement.get());
        }
    }
    catch (const RuntimeError& error)
    {
        std::cerr << "Runtime Error: "
                  << error.what()
                  << std::endl;
    }
}