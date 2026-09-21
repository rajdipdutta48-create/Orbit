#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <variant>
#include <stdexcept>

#include "expr.h"

// Represents a value that can exist at runtime in Orbit.
//
// Currently supported:
// - double
// - bool
//
// More types such as string can be added later.
using Value = std::variant<double, bool>;

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
    // Stores Orbit variables and their current values.
    std::unordered_map<std::string, Value> environment;

    // Evaluates an expression and returns its runtime value.
    Value evaluate(const Expr* expr);

    // Executes a single Orbit statement.
    void execute(const Stmt* stmt);

    // Prints a runtime value to the console.
    void printValue(const Value& value);

public:
    // Executes the complete Orbit program.
    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);
};

#endif