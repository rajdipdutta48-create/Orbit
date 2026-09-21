#include "../include/interpreter.h"
#include <variant>

Value Interpreter::evaluate(const Expr* expr)
{
    // Evaluate a literal.
    // Currently supports:
    // - numbers
    // - true
    // - false
    if (auto literal = dynamic_cast<const Literal*>(expr))
    {
        if (literal->value.type == TokenType::NUMBER)
        {
            return std::stod(literal->value.lexeme);
        }

        if (literal->value.type == TokenType::TRUE)
        {
            return true;
        }

        if (literal->value.type == TokenType::FALSE)
        {
            return false;
        }

        throw RuntimeError("Unknown literal");
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
    // Examples:
    // -10
    // -age
    // !true
    // !false
    if (auto unary = dynamic_cast<const Unary*>(expr))
    {
        Value right = evaluate(unary->right.get());

        if (unary->op.type == TokenType::MINUS)
        {
            if (!std::holds_alternative<double>(right))
            {
                throw RuntimeError(
                    "Operand of '-' must be a number");
            }

            return -std::get<double>(right);
        }

        if (unary->op.type == TokenType::BANG)
        {
            if (!std::holds_alternative<bool>(right))
            {
                throw RuntimeError(
                    "Operand of '!' must be a boolean");
            }

            return !std::get<bool>(right);
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
            if (!std::holds_alternative<double>(left) ||
                !std::holds_alternative<double>(right))
            {
                throw RuntimeError(
                    "Operands of '+' must be numbers");
            }

            return std::get<double>(left) +
                   std::get<double>(right);

        case TokenType::MINUS:
            if (!std::holds_alternative<double>(left) ||
                !std::holds_alternative<double>(right))
            {
                throw RuntimeError(
                    "Operands of '-' must be numbers");
            }

            return std::get<double>(left) -
                   std::get<double>(right);

        case TokenType::STAR:
            if (!std::holds_alternative<double>(left) ||
                !std::holds_alternative<double>(right))
            {
                throw RuntimeError(
                    "Operands of '*' must be numbers");
            }

            return std::get<double>(left) *
                   std::get<double>(right);

        case TokenType::SLASH:
            if (!std::holds_alternative<double>(left) ||
                !std::holds_alternative<double>(right))
            {
                throw RuntimeError(
                    "Operands of '/' must be numbers");
            }

            if (std::get<double>(right) == 0)
            {
                throw RuntimeError("Division by zero");
            }

            return std::get<double>(left) /
                   std::get<double>(right);

        // Comparison operators produce boolean values.
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        {
            if (!std::holds_alternative<double>(left) ||
                !std::holds_alternative<double>(right))
            {
                throw RuntimeError(
                    "Comparison operands must be numbers");
            }

            double leftValue = std::get<double>(left);
            double rightValue = std::get<double>(right);

            if (binary->op.type == TokenType::LESS)
                return leftValue < rightValue;

            if (binary->op.type == TokenType::LESS_EQUAL)
                return leftValue <= rightValue;

            if (binary->op.type == TokenType::GREATER)
                return leftValue > rightValue;

            return leftValue >= rightValue;
        }

        // Equality works with both numbers and booleans.
        case TokenType::EQUAL_EQUAL:
            return left == right;

        case TokenType::NOT_EQUAL:
            return left != right;

        // Logical AND.
        case TokenType::AND:
            if (!std::holds_alternative<bool>(left) ||
                !std::holds_alternative<bool>(right))
            {
                throw RuntimeError(
                    "Operands of '&&' must be booleans");
            }

            return std::get<bool>(left) &&
                   std::get<bool>(right);

        // Logical OR.
        case TokenType::OR:
            if (!std::holds_alternative<bool>(left) ||
                !std::holds_alternative<bool>(right))
            {
                throw RuntimeError(
                    "Operands of '||' must be booleans");
            }

            return std::get<bool>(left) ||
                   std::get<bool>(right);

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

    // Conditional statement:
    //
    // when (condition) {
    //     statements
    // } else {
    //     statements
    // }
    if (auto ifStmt = dynamic_cast<const IfStmt*>(stmt))
    {
        Value condition = evaluate(ifStmt->condition.get());

        // The condition of a when statement must be boolean.
        if (!std::holds_alternative<bool>(condition))
        {
            throw RuntimeError(
                "Condition of 'when' must be a boolean");
        }

        if (std::get<bool>(condition))
        {
            // Execute the when branch.
            for (const auto& statement : ifStmt->thenBranch)
            {
                execute(statement.get());
            }
        }
        else
        {
            // Execute the else branch if it exists.
            for (const auto& statement : ifStmt->elseBranch)
            {
                execute(statement.get());
            }
        }

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