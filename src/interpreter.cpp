#include "../include/interpreter.h"
#include <variant>
#include <cmath>

Value Interpreter::evaluate(const Expr* expr)
{
    // Evaluate a literal.
    //
    // Supported:
    // - numbers
    // - strings
    // - chars
    // - true
    // - false
    if (auto literal = dynamic_cast<const Literal*>(expr))
    {
        if (literal->value.type == TokenType::NUMBER)
        {
            return std::stod(literal->value.lexeme);
        }

        // String literal.
        //
        // Example:
        // "Orbit"
        if (literal->value.type == TokenType::STRING)
        {
            return literal->value.lexeme;
        }

        // Character literal.
        //
        // Example:
        // 'A'
        if (literal->value.type == TokenType::CHAR)
        {
            return literal->value.lexeme[0];
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
    //
    // Examples:
    //
    // name = "Orbit";
    // letter = 'A';
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

    // Evaluate an assignment to an array element.
    //
    // Examples:
    //
    // numbers[1] = 99;
    // names[1] = "Mars";
    // letters[1] = 'Z';
    if (auto indexAssignment =
            dynamic_cast<const IndexAssignment*>(expr))
    {
        Value object = evaluate(indexAssignment->object.get());
        Value index = evaluate(indexAssignment->index.get());
        Value value = evaluate(indexAssignment->value.get());

        // The object being indexed must be an array.
        if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(object))
        {
            throw RuntimeError(
                "Only nebula arrays can be indexed");
        }

        // Array indexes must be numbers.
        if (!std::holds_alternative<double>(index))
        {
            throw RuntimeError(
                "Array index must be a number");
        }

        double indexValue = std::get<double>(index);

        // Array indexes must be whole numbers.
        if (std::floor(indexValue) != indexValue)
        {
            throw RuntimeError(
                "Array index must be an integer");
        }

        if (indexValue < 0)
        {
            throw RuntimeError(
                "Array index cannot be negative");
        }

        auto array =
            std::get<std::shared_ptr<ArrayValue>>(object);

        // Check that the index is inside the array.
        if (indexValue >= array->elements.size())
        {
            throw RuntimeError(
                "Array index out of bounds");
        }

        // Replace the existing element.
        array->elements[
            static_cast<size_t>(indexValue)] = value;

        return value;
    }

    // Evaluate a nebula array literal.
    //
    // Examples:
    //
    // [10, 20, 30]
    // ["Earth", "Mars"]
    // ['A', 'B', 'C']
    //
    // Every element is evaluated first and then stored
    // inside the array.
    if (auto nebula =
            dynamic_cast<const NebulaLiteral*>(expr))
    {
        auto array = std::make_shared<ArrayValue>();

        for (const auto& element : nebula->elements)
        {
            array->elements.push_back(
                evaluate(element.get()));
        }

        return array;
    }

    // Evaluate array indexing.
    //
    // Examples:
    //
    // numbers[0]
    // names[1]
    // letters[2]
    if (auto indexExpr =
            dynamic_cast<const IndexExpr*>(expr))
    {
        Value object = evaluate(indexExpr->object.get());
        Value index = evaluate(indexExpr->index.get());

        // The object being indexed must be an array.
        if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(object))
        {
            throw RuntimeError(
                "Only nebula arrays can be indexed");
        }

        // Array indexes must be numbers.
        if (!std::holds_alternative<double>(index))
        {
            throw RuntimeError(
                "Array index must be a number");
        }

        double indexValue = std::get<double>(index);

        // Array indexes must be whole numbers.
        if (std::floor(indexValue) != indexValue)
        {
            throw RuntimeError(
                "Array index must be an integer");
        }

        if (indexValue < 0)
        {
            throw RuntimeError(
                "Array index cannot be negative");
        }

        auto array =
            std::get<std::shared_ptr<ArrayValue>>(object);

        // Check that the index is inside the array.
        if (indexValue >= array->elements.size())
        {
            throw RuntimeError(
                "Array index out of bounds");
        }

        return array->elements[
            static_cast<size_t>(indexValue)];
    }

    // Evaluate a unary expression.
    //
    // Examples:
    //
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

        switch (binary->op.type)
        {
        // Addition.
        //
        // Number + number
        // String + string
        case TokenType::PLUS:

            if (std::holds_alternative<double>(left) &&
                std::holds_alternative<double>(right))
            {
                return std::get<double>(left) +
                       std::get<double>(right);
            }

            if (std::holds_alternative<std::string>(left) &&
                std::holds_alternative<std::string>(right))
            {
                return std::get<std::string>(left) +
                       std::get<std::string>(right);
            }

            throw RuntimeError(
                "Operands of '+' must both be numbers or both be strings");

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
                throw RuntimeError(
                    "Division by zero");
            }

            return std::get<double>(left) /
                   std::get<double>(right);

        // Comparison operators currently work with numbers.
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

            double leftValue =
                std::get<double>(left);

            double rightValue =
                std::get<double>(right);

            if (binary->op.type == TokenType::LESS)
                return leftValue < rightValue;

            if (binary->op.type == TokenType::LESS_EQUAL)
                return leftValue <= rightValue;

            if (binary->op.type == TokenType::GREATER)
                return leftValue > rightValue;

            return leftValue >= rightValue;
        }

        // Equality works with:
        // - numbers
        // - booleans
        // - strings
        // - chars
        // - arrays
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
            throw RuntimeError(
                "Unknown binary operator");
        }
    }

    // Evaluate a parenthesized expression.
    if (auto grouping =
            dynamic_cast<const Grouping*>(expr))
    {
        return evaluate(
            grouping->expression.get());
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
        std::cout
            << (std::get<bool>(value)
                    ? "true"
                    : "false");
    }

    // Print strings.
    else if (std::holds_alternative<std::string>(value))
    {
        std::cout
            << std::get<std::string>(value);
    }

    // Print characters.
    else if (std::holds_alternative<char>(value))
    {
        std::cout
            << std::get<char>(value);
    }

    // Print nebula arrays.
    else if (
        std::holds_alternative<std::shared_ptr<ArrayValue>>(value))
    {
        auto array =
            std::get<std::shared_ptr<ArrayValue>>(value);

        std::cout << "[";

        for (size_t i = 0;
             i < array->elements.size();
             i++)
        {
            // printValue() does not add a newline,
            // so nested values stay on the same line.
            printValue(array->elements[i]);

            if (i + 1 < array->elements.size())
            {
                std::cout << ", ";
            }
        }

        std::cout << "]";
    }
}

void Interpreter::execute(const Stmt* stmt)
{
    // Variable declaration:
    //
    // dock age = 20;
    // dock name = "Orbit";
    // dock letter = 'O';
    if (auto var =
            dynamic_cast<const VarStmt*>(stmt))
    {
        Value value =
            evaluate(var->initializer.get());

        environment[var->name.lexeme] = value;

        return;
    }

    // Nebula declaration:
    //
    // nebula numbers = [10, 20, 30];
    // nebula names = ["Earth", "Mars"];
    // nebula letters = ['A', 'B', 'C'];
    if (auto nebula =
            dynamic_cast<const NebulaStmt*>(stmt))
    {
        Value value =
            evaluate(nebula->initializer.get());

        // The initializer must actually evaluate to an array.
        if (!std::holds_alternative<
                std::shared_ptr<ArrayValue>>(value))
        {
            throw RuntimeError(
                "Nebula initializer must be an array");
        }

        environment[nebula->name.lexeme] = value;

        return;
    }

    // Print statement:
    //
    // transmit(age);
    // transmit(name);
    // transmit(letter);
    if (auto print =
            dynamic_cast<const PrintStmt*>(stmt))
    {
        Value value =
            evaluate(print->expression.get());

        printValue(value);

        // Add the newline only once, after the complete
        // value has been printed.
        std::cout << std::endl;

        return;
    }

    // Input statement.
    //
    // receive() accepts input according to the
    // type of the target variable or array element.
    if (auto input =
            dynamic_cast<const InputStmt*>(stmt))
    {
        // receive(variable);
        if (auto variable =
                dynamic_cast<const Variable*>(
                    input->target.get()))
        {
            auto it =
                environment.find(
                    variable->name.lexeme);

            if (it == environment.end())
            {
                throw RuntimeError(
                    "Undefined variable '" +
                    variable->name.lexeme +
                    "'");
            }

            Value& target = it->second;

            std::cout << "Enter value: ";

            // Number input.
            if (std::holds_alternative<double>(target))
            {
                double value;

                std::cin >> value;

                if (std::cin.fail())
                {
                    std::cin.clear();
                    std::string invalid;
                    std::cin >> invalid;

                    throw RuntimeError(
                        "receive() expects a number");
                }

                target = value;
            }

            // String input.
            //
            // Input is read until whitespace.
            else if (std::holds_alternative<std::string>(target))
            {
                std::string value;

                std::cin >> value;

                if (std::cin.fail())
                {
                    throw RuntimeError(
                        "receive() expects a string");
                }

                target = value;
            }

            // Character input.
            else if (std::holds_alternative<char>(target))
            {
                char value;

                std::cin >> value;

                if (std::cin.fail())
                {
                    throw RuntimeError(
                        "receive() expects a character");
                }

                target = value;
            }

            else
            {
                throw RuntimeError(
                    "receive() does not support this variable type");
            }

            return;
        }

        // receive(numbers[index]);
        if (auto indexExpr =
                dynamic_cast<const IndexExpr*>(
                    input->target.get()))
        {
            Value object =
                evaluate(indexExpr->object.get());

            Value index =
                evaluate(indexExpr->index.get());

            // The object being indexed must be an array.
            if (!std::holds_alternative<
                    std::shared_ptr<ArrayValue>>(object))
            {
                throw RuntimeError(
                    "Only nebula arrays can be indexed");
            }

            // Array indexes must be numbers.
            if (!std::holds_alternative<double>(index))
            {
                throw RuntimeError(
                    "Array index must be a number");
            }

            double indexValue =
                std::get<double>(index);

            // Array indexes must be whole numbers.
            if (std::floor(indexValue) != indexValue)
            {
                throw RuntimeError(
                    "Array index must be an integer");
            }

            if (indexValue < 0)
            {
                throw RuntimeError(
                    "Array index cannot be negative");
            }

            auto array =
                std::get<
                    std::shared_ptr<ArrayValue>>(object);

            // Check that the index is inside the array.
            if (indexValue >= array->elements.size())
            {
                throw RuntimeError(
                    "Array index out of bounds");
            }

            Value& target =
                array->elements[
                    static_cast<size_t>(indexValue)];

            std::cout << "Enter value: ";

            // Number input.
            if (std::holds_alternative<double>(target))
            {
                double value;

                std::cin >> value;

                if (std::cin.fail())
                {
                    std::cin.clear();
                    std::string invalid;
                    std::cin >> invalid;

                    throw RuntimeError(
                        "receive() expects a number");
                }

                target = value;
            }

            // String input.
            else if (std::holds_alternative<std::string>(target))
            {
                std::string value;

                std::cin >> value;

                if (std::cin.fail())
                {
                    throw RuntimeError(
                        "receive() expects a string");
                }

                target = value;
            }

            // Character input.
            else if (std::holds_alternative<char>(target))
            {
                char value;

                std::cin >> value;

                if (std::cin.fail())
                {
                    throw RuntimeError(
                        "receive() expects a character");
                }

                target = value;
            }

            else
            {
                throw RuntimeError(
                    "receive() does not support this array element type");
            }

            return;
        }

        throw RuntimeError(
            "Invalid receive target");
    }

    // Conditional statement:
    //
    // when (condition) {
    //     statements
    // } else {
    //     statements
    // }
    if (auto ifStmt =
            dynamic_cast<const IfStmt*>(stmt))
    {
        Value condition =
            evaluate(ifStmt->condition.get());

        // The condition of a when statement must be boolean.
        if (!std::holds_alternative<bool>(condition))
        {
            throw RuntimeError(
                "Condition of 'when' must be boolean");
        }

        if (std::get<bool>(condition))
        {
            // Execute the when branch.
            for (const auto& statement :
                 ifStmt->thenBranch)
            {
                execute(statement.get());
            }
        }
        else
        {
            // Execute the else branch if it exists.
            for (const auto& statement :
                 ifStmt->elseBranch)
            {
                execute(statement.get());
            }
        }

        return;
    }

    // While loop:
    //
    // orbiting (condition) {
    //     statements
    // }
    if (auto whileStmt =
            dynamic_cast<const WhileStmt*>(stmt))
    {
        while (true)
        {
            Value condition =
                evaluate(
                    whileStmt->condition.get());

            // The condition of an orbiting statement
            // must be boolean.
            if (!std::holds_alternative<bool>(condition))
            {
                throw RuntimeError(
                    "Condition of 'orbiting' must be boolean");
            }

            // Stop when the condition becomes false.
            if (!std::get<bool>(condition))
            {
                break;
            }

            // Execute every statement in the loop body.
            for (const auto& statement :
                 whileStmt->body)
            {
                execute(statement.get());
            }
        }

        return;
    }

    // Standalone expression:
    //
    // age = 25;
    // name = "Orbit";
    // letter = 'A';
    if (auto expressionStmt =
            dynamic_cast<const ExpressionStmt*>(stmt))
    {
        evaluate(
            expressionStmt->expression.get());

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
        std::cerr
            << "Runtime Error: "
            << error.what()
            << std::endl;
    }
}