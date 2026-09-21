#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

#include "expr.h"

class Interpreter
{
private:
    // Stores Orbit variables and their current values.
    std::unordered_map<std::string, double> environment;

    // Evaluates an expression and returns its numeric value.
    double evaluate(const Expr* expr);

    // Executes a single Orbit statement.
    void execute(const Stmt* stmt);

public:
    // Executes the complete Orbit program.
    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);
};

#endif