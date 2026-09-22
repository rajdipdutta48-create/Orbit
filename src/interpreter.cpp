#include "../include/interpreter.h"

#include <cmath>
#include <iostream>
#include <variant>

// Returns true when an expression contains a numeric value that
// is fractional (for example 55.6) anywhere inside its AST.
// This is used by the modulo operator so a decimal is rejected
// before the modulo operation is performed.
static bool containsDecimalValue(const Expr* expr)
{
    if (expr == nullptr)
    {
        return false;
    }

    // Numeric/string/char/bool literal.
    if (auto literal = dynamic_cast<const Literal*>(expr))
    {
        if (literal->value.type == TokenType::NUMBER)
        {
            double value =
                std::stod(literal->value.lexeme);

            return std::floor(value) != value;
        }

        return false;
    }

    // Assignment expression.
    if (auto assignment =
            dynamic_cast<const Assignment*>(expr))
    {
        return containsDecimalValue(
            assignment->value.get());
    }

    // Array element assignment expression.
    if (auto indexAssignment =
            dynamic_cast<const IndexAssignment*>(expr))
    {
        return containsDecimalValue(
                   indexAssignment->object.get()) ||
               containsDecimalValue(
                   indexAssignment->index.get()) ||
               containsDecimalValue(
                   indexAssignment->value.get());
    }

    // Nebula array literal.
    if (auto nebula =
            dynamic_cast<const NebulaLiteral*>(expr))
    {
        for (const auto& element :
             nebula->elements)
        {
            if (containsDecimalValue(element.get()))
            {
                return true;
            }
        }

        return false;
    }

    // Array indexing.
    if (auto indexExpr =
            dynamic_cast<const IndexExpr*>(expr))
    {
        return containsDecimalValue(
                   indexExpr->object.get()) ||
               containsDecimalValue(
                   indexExpr->index.get());
    }

    // Function call.
    if (auto call =
            dynamic_cast<const Call*>(expr))
    {
        if (containsDecimalValue(call->callee.get()))
        {
            return true;
        }

        for (const auto& argument :
             call->arguments)
        {
            if (containsDecimalValue(argument.get()))
            {
                return true;
            }
        }

        return false;
    }

    // Unary expression.
    if (auto unary =
            dynamic_cast<const Unary*>(expr))
    {
        return containsDecimalValue(
            unary->right.get());
    }

    // Binary expression.
    if (auto binary =
            dynamic_cast<const Binary*>(expr))
    {
        return containsDecimalValue(
                   binary->left.get()) ||
               containsDecimalValue(
                   binary->right.get());
    }

    // Grouping expression.
    if (auto grouping =
            dynamic_cast<const Grouping*>(expr))
    {
        return containsDecimalValue(
            grouping->expression.get());
    }

    // Variable lookup does not contain a literal value
    // in the AST. Its runtime value is checked later.
    return false;
}

Interpreter::Interpreter()
    : functionDepth(0)
{
    // The first environment is always the global scope.
    environments.emplace_back();
}

Value Interpreter::lookup(const std::string& name)
{
    // Search from the innermost scope to the global scope.
    for (auto it = environments.rbegin();
         it != environments.rend();
         ++it)
    {
        auto found = it->find(name);

        if (found != it->end())
        {
            return found->second;
        }
    }

    throw RuntimeError(
        "Undefined variable '" + name + "'");
}

void Interpreter::assign(
    const std::string& name,
    const Value& value)
{
    // Assignment searches from the innermost scope
    // outward so local variables and parameters can
    // be changed without affecting unrelated scopes.
    for (auto it = environments.rbegin();
         it != environments.rend();
         ++it)
    {
        auto found = it->find(name);

        if (found != it->end())
        {
            found->second = value;
            return;
        }
    }

    throw RuntimeError(
        "Undefined variable '" + name + "'");
}

void Interpreter::define(
    const std::string& name,
    const Value& value)
{
    // New variables belong to the current scope.
    environments.back()[name] = value;
}

Value Interpreter::evaluate(const Expr* expr)
{
    if (expr == nullptr)
    {
        throw RuntimeError("Invalid expression");
    }

    // ---------------------------------------------------------
    // Literal
    // ---------------------------------------------------------

    if (auto literal =
            dynamic_cast<const Literal*>(expr))
    {
        if (literal->value.type == TokenType::NUMBER)
        {
            return std::stod(
                literal->value.lexeme);
        }

        if (literal->value.type == TokenType::STRING)
        {
            return literal->value.lexeme;
        }

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

    // ---------------------------------------------------------
    // Variable lookup
    // ---------------------------------------------------------

    if (auto variable =
            dynamic_cast<const Variable*>(expr))
    {
        return lookup(
            variable->name.lexeme);
    }

    // ---------------------------------------------------------
    // Variable assignment
    // ---------------------------------------------------------

    if (auto assignment =
            dynamic_cast<const Assignment*>(expr))
    {
        Value value =
            evaluate(assignment->value.get());

        assign(
            assignment->name.lexeme,
            value);

        return value;
    }

    // ---------------------------------------------------------
    // Array element assignment
    // ---------------------------------------------------------

    if (auto indexAssignment =
            dynamic_cast<const IndexAssignment*>(expr))
    {
        Value object =
            evaluate(indexAssignment->object.get());

        Value index =
            evaluate(indexAssignment->index.get());

        Value value =
            evaluate(indexAssignment->value.get());

        if (!std::holds_alternative<
                std::shared_ptr<ArrayValue>>(object))
        {
            throw RuntimeError(
                "Only nebula arrays can be indexed");
        }

        if (!std::holds_alternative<double>(index))
        {
            throw RuntimeError(
                "Array index must be a number");
        }

        double indexValue =
            std::get<double>(index);

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
            std::get<std::shared_ptr<ArrayValue>>(
                object);

        if (indexValue >= array->elements.size())
        {
            throw RuntimeError(
                "Array index out of bounds");
        }

        array->elements[
            static_cast<size_t>(indexValue)] = value;

        return value;
    }

    // ---------------------------------------------------------
    // Nebula literal
    // ---------------------------------------------------------

    if (auto nebula =
            dynamic_cast<const NebulaLiteral*>(expr))
    {
        auto array =
            std::make_shared<ArrayValue>();

        for (const auto& element :
             nebula->elements)
        {
            array->elements.push_back(
                evaluate(element.get()));
        }

        return array;
    }

    // ---------------------------------------------------------
    // Array indexing
    // ---------------------------------------------------------

    if (auto indexExpr =
            dynamic_cast<const IndexExpr*>(expr))
    {
        Value object =
            evaluate(indexExpr->object.get());

        Value index =
            evaluate(indexExpr->index.get());

        if (!std::holds_alternative<
                std::shared_ptr<ArrayValue>>(object))
        {
            throw RuntimeError(
                "Only nebula arrays can be indexed");
        }

        if (!std::holds_alternative<double>(index))
        {
            throw RuntimeError(
                "Array index must be a number");
        }

        double indexValue =
            std::get<double>(index);

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
            std::get<std::shared_ptr<ArrayValue>>(
                object);

        if (indexValue >= array->elements.size())
        {
            throw RuntimeError(
                "Array index out of bounds");
        }

        return array->elements[
            static_cast<size_t>(indexValue)];
    }

    // ---------------------------------------------------------
    // Function call
    // ---------------------------------------------------------

    if (auto call =
            dynamic_cast<const Call*>(expr))
    {
        Value callee =
            evaluate(call->callee.get());

        if (!std::holds_alternative<
                std::shared_ptr<FunctionValue>>(callee))
        {
            throw RuntimeError(
                "Only warp functions can be called");
        }

        std::vector<Value> arguments;

        for (const auto& argument :
             call->arguments)
        {
            arguments.push_back(
                evaluate(argument.get()));
        }

        auto function =
            std::get<std::shared_ptr<FunctionValue>>(
                callee);

        return callFunction(
            function,
            arguments);
    }

    // ---------------------------------------------------------
    // Unary expressions
    // ---------------------------------------------------------

    if (auto unary =
            dynamic_cast<const Unary*>(expr))
    {
        Value right =
            evaluate(unary->right.get());

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

        throw RuntimeError(
            "Unknown unary operator");
    }

    // ---------------------------------------------------------
    // Binary expressions
    // ---------------------------------------------------------

    if (auto binary =
            dynamic_cast<const Binary*>(expr))
    {
        // For modulo, reject any fractional numeric value anywhere
        // inside either operand's AST before evaluating the modulo.
        if (binary->op.type == TokenType::MODULO)
        {
            if (containsDecimalValue(binary->left.get()) ||
                containsDecimalValue(binary->right.get()))
            {
                throw RuntimeError(
                    "Operands of '%' must be integers");
            }
        }

        Value left =
            evaluate(binary->left.get());

        Value right =
            evaluate(binary->right.get());

        switch (binary->op.type)
        {
        // -----------------------------------------------------
        // Addition
        // -----------------------------------------------------

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

        // -----------------------------------------------------
        // Subtraction
        // -----------------------------------------------------

        case TokenType::MINUS:

            if (!std::holds_alternative<double>(left) ||
                !std::holds_alternative<double>(right))
            {
                throw RuntimeError(
                    "Operands of '-' must be numbers");
            }

            return std::get<double>(left) -
                   std::get<double>(right);

        // -----------------------------------------------------
        // Multiplication
        // -----------------------------------------------------

        case TokenType::STAR:

            if (!std::holds_alternative<double>(left) ||
                !std::holds_alternative<double>(right))
            {
                throw RuntimeError(
                    "Operands of '*' must be numbers");
            }

            return std::get<double>(left) *
                   std::get<double>(right);

        // -----------------------------------------------------
        // Division
        // -----------------------------------------------------

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

        // -----------------------------------------------------
        // Modulo
        //
        // Orbit's % operator is integer modulo.
        // Any fractional value anywhere inside either operand
        // has already been rejected by the AST check above.
        //
        // 17 % 5       -> 2
        // 10.0 % 3     -> 1
        // 10.5 % 3     -> Runtime Error
        // -----------------------------------------------------

        case TokenType::MODULO:
        {
            if (!std::holds_alternative<double>(left) ||
                !std::holds_alternative<double>(right))
            {
                throw RuntimeError(
                    "Operands of '%' must be numbers");
            }

            double leftValue =
                std::get<double>(left);

            double rightValue =
                std::get<double>(right);

            // Runtime check is required for values coming from
            // variables, function calls, input, or other expressions.
            if (std::floor(leftValue) != leftValue ||
                std::floor(rightValue) != rightValue)
            {
                throw RuntimeError(
                    "Operands of '%' must be integers");
            }

            if (rightValue == 0)
            {
                throw RuntimeError(
                    "Modulo by zero");
            }

            // The actual modulo operation uses C++ integer %.
            long long leftInteger =
                static_cast<long long>(leftValue);

            long long rightInteger =
                static_cast<long long>(rightValue);

            return static_cast<double>(
                leftInteger % rightInteger);
        }

        // -----------------------------------------------------
        // Comparison
        // -----------------------------------------------------

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
            {
                return leftValue < rightValue;
            }

            if (binary->op.type == TokenType::LESS_EQUAL)
            {
                return leftValue <= rightValue;
            }

            if (binary->op.type == TokenType::GREATER)
            {
                return leftValue > rightValue;
            }

            return leftValue >= rightValue;
        }

        // -----------------------------------------------------
        // Equality
        // -----------------------------------------------------

        case TokenType::EQUAL_EQUAL:
            return left == right;

        case TokenType::NOT_EQUAL:
            return left != right;

        // -----------------------------------------------------
        // Logical AND
        // -----------------------------------------------------

        case TokenType::AND:

            if (!std::holds_alternative<bool>(left) ||
                !std::holds_alternative<bool>(right))
            {
                throw RuntimeError(
                    "Operands of '&&' must be booleans");
            }

            return std::get<bool>(left) &&
                   std::get<bool>(right);

        // -----------------------------------------------------
        // Logical OR
        // -----------------------------------------------------

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

    // ---------------------------------------------------------
    // Grouping
    // ---------------------------------------------------------

    if (auto grouping =
            dynamic_cast<const Grouping*>(expr))
    {
        return evaluate(
            grouping->expression.get());
    }

    throw RuntimeError(
        "Unknown expression");
}

void Interpreter::printValue(const Value& value)
{
    if (std::holds_alternative<double>(value))
    {
        std::cout << std::get<double>(value);
    }

    else if (std::holds_alternative<bool>(value))
    {
        std::cout
            << (std::get<bool>(value)
                    ? "true"
                    : "false");
    }

    else if (std::holds_alternative<std::string>(value))
    {
        std::cout
            << std::get<std::string>(value);
    }

    else if (std::holds_alternative<char>(value))
    {
        std::cout
            << std::get<char>(value);
    }

    else if (
        std::holds_alternative<
            std::shared_ptr<ArrayValue>>(value))
    {
        auto array =
            std::get<std::shared_ptr<ArrayValue>>(
                value);

        std::cout << "[";

        for (size_t i = 0;
             i < array->elements.size();
             i++)
        {
            printValue(array->elements[i]);

            if (i + 1 < array->elements.size())
            {
                std::cout << ", ";
            }
        }

        std::cout << "]";
    }

    else if (
        std::holds_alternative<
            std::shared_ptr<FunctionValue>>(value))
    {
        std::cout << "<warp function>";
    }
}

Value Interpreter::callFunction(
    const std::shared_ptr<FunctionValue>& function,
    const std::vector<Value>& arguments)
{
    if (!function ||
        function->declaration == nullptr)
    {
        throw RuntimeError(
            "Invalid warp function");
    }

    const FunctionStmt* declaration =
        function->declaration;

    // Check argument count.
    if (arguments.size() != declaration->params.size())
    {
        throw RuntimeError(
            "Function '" +
            declaration->name.lexeme +
            "' expects " +
            std::to_string(declaration->params.size()) +
            " argument(s), but received " +
            std::to_string(arguments.size()));
    }

    // Create the function's local scope.
    environments.emplace_back();

    functionDepth++;

    // Bind each argument to its parameter.
    for (size_t i = 0;
         i < declaration->params.size();
         i++)
    {
        define(
            declaration->params[i].lexeme,
            arguments[i]);
    }

    try
    {
        // Execute the function body inside the new scope.
        for (const auto& statement :
             declaration->body)
        {
            execute(statement.get());
        }
    }
    catch (const ReturnSignal& signal)
    {
        functionDepth--;
        environments.pop_back();

        // A bare return does not carry a value.
        //
        // Orbit functions currently use Value as their
        // expression result type, so a bare return produces
        // the numeric value 0 when the call is used as an
        // expression.
        //
        // Calling the function as a standalone expression
        // simply ignores this result.
        if (!signal.value.has_value())
        {
            return 0.0;
        }

        return signal.value.value();
    }

    functionDepth--;
    environments.pop_back();

    // Reaching the end of a warp without returning a value
    // produces 0 as its default result.
    return 0.0;
}

void Interpreter::executeBlock(
    const std::vector<std::unique_ptr<Stmt>>& statements)
{
    // Create a nested lexical scope.
    environments.emplace_back();

    try
    {
        for (const auto& statement : statements)
        {
            execute(statement.get());
        }
    }
    catch (...)
    {
        environments.pop_back();
        throw;
    }

    environments.pop_back();
}

void Interpreter::execute(const Stmt* stmt)
{
    if (stmt == nullptr)
    {
        return;
    }

    // ---------------------------------------------------------
    // Variable declaration
    // ---------------------------------------------------------

    if (auto var =
            dynamic_cast<const VarStmt*>(stmt))
    {
        Value value =
            evaluate(var->initializer.get());

        define(
            var->name.lexeme,
            value);

        return;
    }

    // ---------------------------------------------------------
    // Nebula declaration
    // ---------------------------------------------------------

    if (auto nebula =
            dynamic_cast<const NebulaStmt*>(stmt))
    {
        Value value =
            evaluate(nebula->initializer.get());

        if (!std::holds_alternative<
                std::shared_ptr<ArrayValue>>(value))
        {
            throw RuntimeError(
                "Nebula initializer must be an array");
        }

        define(
            nebula->name.lexeme,
            value);

        return;
    }

    // ---------------------------------------------------------
    // Warp function declaration
    // ---------------------------------------------------------

    if (auto function =
            dynamic_cast<const FunctionStmt*>(stmt))
    {
        auto functionValue =
            std::make_shared<FunctionValue>(
                function);

        define(
            function->name.lexeme,
            functionValue);

        return;
    }

    // ---------------------------------------------------------
    // Return statement
    // ---------------------------------------------------------

    if (auto returnStmt =
            dynamic_cast<const ReturnStmt*>(stmt))
    {
        // return is only valid inside a warp function.
        if (functionDepth <= 0)
        {
            throw RuntimeError(
                "'return' can only be used inside a warp function");
        }

        // Bare return.
        if (!returnStmt->value)
        {
            throw ReturnSignal(
                std::nullopt);
        }

        // Evaluate the return expression before
        // leaving the current function scope.
        Value value =
            evaluate(
                returnStmt->value.get());

        throw ReturnSignal(
            value);
    }

    // ---------------------------------------------------------
    // Print statement
    // ---------------------------------------------------------

    if (auto print =
            dynamic_cast<const PrintStmt*>(stmt))
    {
        Value value =
            evaluate(print->expression.get());

        printValue(value);

        std::cout << std::endl;

        return;
    }

    // ---------------------------------------------------------
    // Input statement
    // ---------------------------------------------------------

    if (auto input =
            dynamic_cast<const InputStmt*>(stmt))
    {
        // receive(variable);
        if (auto variable =
                dynamic_cast<const Variable*>(
                    input->target.get()))
        {
            Value target =
                lookup(variable->name.lexeme);

            std::cout << "Enter value: ";

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

                assign(
                    variable->name.lexeme,
                    value);
            }

            else if (
                std::holds_alternative<std::string>(
                    target))
            {
                std::string value;

                std::cin >> value;

                if (std::cin.fail())
                {
                    throw RuntimeError(
                        "receive() expects a string");
                }

                assign(
                    variable->name.lexeme,
                    value);
            }

            else if (
                std::holds_alternative<char>(target))
            {
                char value;

                std::cin >> value;

                if (std::cin.fail())
                {
                    throw RuntimeError(
                        "receive() expects a character");
                }

                assign(
                    variable->name.lexeme,
                    value);
            }

            else
            {
                throw RuntimeError(
                    "receive() does not support this variable type");
            }

            return;
        }

        // receive(numbers[index]);
        //
        // evaluate(object) already supports nested indexing,
        // so this also works for:
        //
        // receive(matrix[0][1]);
        // receive(cube[1][0][1]);
        if (auto indexExpr =
                dynamic_cast<const IndexExpr*>(
                    input->target.get()))
        {
            Value object =
                evaluate(indexExpr->object.get());

            Value index =
                evaluate(indexExpr->index.get());

            if (!std::holds_alternative<
                    std::shared_ptr<ArrayValue>>(object))
            {
                throw RuntimeError(
                    "Only nebula arrays can be indexed");
            }

            if (!std::holds_alternative<double>(index))
            {
                throw RuntimeError(
                    "Array index must be a number");
            }

            double indexValue =
                std::get<double>(index);

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
                    std::shared_ptr<ArrayValue>>(
                        object);

            if (indexValue >= array->elements.size())
            {
                throw RuntimeError(
                    "Array index out of bounds");
            }

            Value& target =
                array->elements[
                    static_cast<size_t>(indexValue)];

            std::cout << "Enter value: ";

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

            else if (
                std::holds_alternative<std::string>(target))
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

            else if (
                std::holds_alternative<char>(target))
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

    // ---------------------------------------------------------
    // when
    // ---------------------------------------------------------

    if (auto ifStmt =
            dynamic_cast<const IfStmt*>(stmt))
    {
        Value condition =
            evaluate(ifStmt->condition.get());

        if (!std::holds_alternative<bool>(condition))
        {
            throw RuntimeError(
                "Condition of 'when' must be boolean");
        }

        if (std::get<bool>(condition))
        {
            for (const auto& statement :
                 ifStmt->thenBranch)
            {
                execute(statement.get());
            }
        }
        else
        {
            for (const auto& statement :
                 ifStmt->elseBranch)
            {
                execute(statement.get());
            }
        }

        return;
    }

    // ---------------------------------------------------------
    // orbiting
    // ---------------------------------------------------------

    if (auto whileStmt =
            dynamic_cast<const WhileStmt*>(stmt))
    {
        while (true)
        {
            Value condition =
                evaluate(
                    whileStmt->condition.get());

            if (!std::holds_alternative<bool>(condition))
            {
                throw RuntimeError(
                    "Condition of 'orbiting' must be boolean");
            }

            if (!std::get<bool>(condition))
            {
                break;
            }

            for (const auto& statement :
                 whileStmt->body)
            {
                execute(statement.get());
            }
        }

        return;
    }

    // ---------------------------------------------------------
    // Standalone expression
    // ---------------------------------------------------------

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
        // Register all top-level warp functions first.
        //
        // This allows:
        //
        // greet();
        //
        // to work even when the warp declaration appears
        // later in the source file.
        for (const auto& statement :
             statements)
        {
            if (auto function =
                    dynamic_cast<const FunctionStmt*>(
                        statement.get()))
            {
                auto functionValue =
                    std::make_shared<FunctionValue>(
                        function);

                define(
                    function->name.lexeme,
                    functionValue);
            }
        }

        // Execute the actual program.
        //
        // Function declarations are already registered,
        // so they do not need to execute a second time.
        for (const auto& statement :
             statements)
        {
            if (dynamic_cast<const FunctionStmt*>(
                    statement.get()))
            {
                continue;
            }

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