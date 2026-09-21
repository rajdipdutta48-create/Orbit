#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>

#include "token.h"
#include "expr.h"

class Parser
{
private:
    std::vector<Token> tokens;
    int current;

    bool isAtEnd();

    Token peek();
    Token previous();
    Token advance();

    bool check(TokenType type);
    bool match(TokenType type);

    // Expression parsing
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> expression();

    // Statement parsing
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> inputStatement();
    std::unique_ptr<Stmt> expressionStatement();

public:
    Parser(const std::vector<Token>& tokens);

    std::vector<std::unique_ptr<Stmt>> parse();
};

#endif