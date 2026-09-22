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

// Forward declaration because an ArrayValue contains Orbit Values.
struct ArrayValue;

// Represents a value that can exist at runtime in Orbit.
//
// Currently supported:
// - double
// - bool
// - string
// - char
// - array
//
// Numbers are stored as double, so Orbit supports both
// integer and decimal values.
//
// Examples:
//
// dock age = 20;
// dock price = 99.99;
using Value = std::variant<
    double,
    bool,
    std::string,
    char,
    std::shared_ptr<ArrayValue>>;

// Represents an Orbit nebula array.
//
// Example:
//
// nebula numbers = [10, 20, 30];
//
// Each element is itself a Value, so arrays can contain:
// - numbers
// - decimal numbers
// - booleans
// - strings
// - characters
// - other arrays
//
// Because an element can itself be an ArrayValue, Orbit
// naturally supports nested and multidimensional arrays.
//
// Example 2D array:
//
// nebula matrix = [
//     [1.5, 2.5],
//     [3.5, 4.5]
// ];
//
// Access:
//
// matrix[0][1]
//
// Example 3D array:
//
// nebula cube = [
//     [
//         [1, 2],
//         [3, 4]
//     ],
//     [
//         [5, 6],
//         [7, 8]
//     ]
// ];
//
// Access:
//
// cube[1][0][1]
struct ArrayValue
{
    std::vector<Value> elements;
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
    // Stores Orbit variables and their current values.
    std::unordered_map<std::string, Value> environment;

    // Evaluates an expression and returns its runtime value.
    Value evaluate(const Expr* expr);

    // Executes a single Orbit statement.
    void execute(const Stmt* stmt);

    // Prints a runtime value to the console.
    //
    // Arrays are printed recursively, so nested arrays
    // are displayed correctly.
    //
    // Example:
    //
    // [[1, 2], [3, 4]]
    void printValue(const Value& value);

public:
    // Executes the complete Orbit program.
    void interpret(
        const std::vector<std::unique_ptr<Stmt>>& statements);
};

#endif