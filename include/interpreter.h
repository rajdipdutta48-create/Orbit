#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <variant>
#include <stdexcept>
#include <optional>
#include <cmath>

#include "expr.h"

// Forward declaration because an ArrayValue contains Orbit Values.
struct ArrayValue;

// Forward declaration for callable Orbit functions.
struct FunctionValue;

// Represents a value that can exist at runtime in Orbit.
//
// Currently supported:
// - double
// - bool
// - string
// - char
// - array
// - function
//
// Numbers are stored as double, so Orbit supports both
// integer and decimal values.
using Value = std::variant<
    double,
    bool,
    std::string,
    char,
    std::shared_ptr<ArrayValue>,
    std::shared_ptr<FunctionValue>>;

// Represents an Orbit nebula array.
//
// Each element is a Value, so arrays can contain:
// - numbers
// - decimal numbers
// - booleans
// - strings
// - characters
// - other arrays
//
// Because an element can itself be an ArrayValue, Orbit
// naturally supports nested and multidimensional arrays.
struct ArrayValue
{
    std::vector<Value> elements;
};

// Represents a user-defined Orbit function.
//
// Example:
//
// warp add(a, b) {
//     return a + b;
// }
//
// The FunctionStmt itself remains owned by the parsed AST.
// FunctionValue only keeps a pointer to that declaration.
struct FunctionValue
{
    const FunctionStmt* declaration;

    explicit FunctionValue(const FunctionStmt* declaration)
        : declaration(declaration)
    {
    }
};

// Internal control-flow signal used to move a return value
// out of a function body.
//
// This is not shown to the Orbit user as a runtime error.
// It is caught by the function-call mechanism.
struct ReturnSignal
{
    std::optional<Value> value;

    explicit ReturnSignal(std::optional<Value> value)
        : value(std::move(value))
    {
    }
};

// Represents an error that occurs while executing Orbit code.
class RuntimeError : public std::runtime_error
{
public:
    RuntimeError(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class Interpreter
{
private:
    // The first environment is the global scope.
    //
    // Additional environments are pushed when a warp
    // function is called.
    std::vector<
        std::unordered_map<std::string, Value>>
        environments;

    // Tracks whether execution is currently inside
    // a warp function.
    //
    // This lets Orbit reject:
    //
    // return 10;
    //
    // at the top level.
    int functionDepth;

    // Evaluates an expression and returns its runtime value.
    Value evaluate(const Expr* expr);

    // Executes a single Orbit statement.
    void execute(const Stmt* stmt);

    // Executes a list of statements.
    void executeBlock(
        const std::vector<std::unique_ptr<Stmt>>& statements);

    // Looks up a variable starting from the innermost
    // active scope and moving outward.
    Value lookup(const std::string& name);

    // Assigns to an existing variable, searching from
    // the innermost scope outward.
    void assign(
        const std::string& name,
        const Value& value);

    // Creates a variable in the current scope.
    void define(
        const std::string& name,
        const Value& value);

    // Calls a user-defined warp function.
    Value callFunction(
        const std::shared_ptr<FunctionValue>& function,
        const std::vector<Value>& arguments);

    // Prints a runtime value to the console.
    //
    // Arrays are printed recursively, so nested arrays
    // are displayed correctly.
    void printValue(const Value& value);

public:
    Interpreter();

    // Executes the complete Orbit program.
    void interpret(
        const std::vector<std::unique_ptr<Stmt>>& statements);
};

#endif