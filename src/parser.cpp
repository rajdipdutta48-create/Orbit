#include "../include/parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token> &tokens)
    : tokens(tokens), current(0)
{
}

Token Parser::peek() // Returns the current token without moving to the next one.
{
    return tokens[current];
}

Token Parser::previous() // Returns the token just before the current position.
{
    return tokens[current - 1];
}

bool Parser::isAtEnd() // Checks whether the parser has reached the END_OF_FILE token.
{
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::advance() // Moves to the next token and returns the token that was just consumed.
{
    if (!isAtEnd())
    {
        current++;
    }

    return previous();
}

bool Parser::check(TokenType type) // Checks whether the current token is of the expected type.
{
    if (isAtEnd())
    {
        return false;
    }

    return peek().type == type;
}

bool Parser::match(TokenType type)
/*
  If the current token matches the expected type,
  consume it by advancing and return true.

  Otherwise, leave the parser unchanged and return false.
*/
{
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}

std::unique_ptr<Expr> Parser::primary()
/*
  Parses the simplest expressions.

  Currently supports:
  - Number literals
  - Parenthesized expressions
  - Variables / identifiers
*/
{
    // Example: 123
    if (match(TokenType::NUMBER))
    {
        return std::make_unique<Literal>(previous());
    }

    // Example: (10 + 20)
    if (match(TokenType::LEFT_PAREN))
    {
        auto expr = expression();

        match(TokenType::RIGHT_PAREN);

        return std::make_unique<Grouping>(std::move(expr));
    }

    // Example: age
    if (match(TokenType::IDENTIFIER))
    {
        return std::make_unique<Variable>(previous());
    }

    // No valid primary expression was found.
    return nullptr;
}

std::unique_ptr<Expr> Parser::unary()
/*
  Parses unary expressions.

  Examples:
      -10
      -age
      -(10 + 5)

  Unary operators have higher precedence than
  multiplication, division, addition and subtraction.
*/
{
    if (match(TokenType::MINUS))
    {
        Token op = previous();

        auto right = unary();

        return std::make_unique<Unary>(
            op,
            std::move(right));
    }

    return primary();
}

std::unique_ptr<Expr> Parser::factor()
/*
  Parses multiplication and division.

  factor handles:
      *
      /

  Because factor() is called before term(),
  multiplication and division get higher precedence
  than addition and subtraction.
*/
{
    auto left = unary();

    while (match(TokenType::STAR) || match(TokenType::SLASH))
    {
        Token op = previous();

        auto right = unary();

        left = std::make_unique<Binary>(
            std::move(left),
            op,
            std::move(right));
    }

    return left;
}

std::unique_ptr<Expr> Parser::term()
/*
  Parses addition and subtraction.

  term handles:
      +
      -

  It calls factor() first, so expressions such as:

      10 + 20 * 5

  are parsed as:

      10 + (20 * 5)
*/
{
    auto left = factor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS))
    {
        Token op = previous();

        auto right = factor();

        left = std::make_unique<Binary>(
            std::move(left),
            op,
            std::move(right));
    }

    return left;
}

std::unique_ptr<Expr> Parser::comparison()
/*
  Parses comparison expressions.

  comparison handles:

      <
      <=
      >
      >=
*/
{
    auto left = term();

    while (match(TokenType::LESS) ||
           match(TokenType::LESS_EQUAL) ||
           match(TokenType::GREATER) ||
           match(TokenType::GREATER_EQUAL))
    {
        Token op = previous();

        auto right = term();

        left = std::make_unique<Binary>(
            std::move(left),
            op,
            std::move(right));
    }

    return left;
}

std::unique_ptr<Expr> Parser::equality()
/*
  Parses equality expressions.

  equality handles:

      ==
      !=
*/
{
    auto left = comparison();

    while (match(TokenType::EQUAL_EQUAL) ||
           match(TokenType::NOT_EQUAL))
    {
        Token op = previous();

        auto right = comparison();

        left = std::make_unique<Binary>(
            std::move(left),
            op,
            std::move(right));
    }

    return left;
}

std::unique_ptr<Expr> Parser::assignment()
/*
  Parses assignment expressions.

  Example:

      age = 25;

  Assignment has lower precedence than
  equality and comparison.

  The left side must be a variable.
*/
{
    auto left = equality();

    if (match(TokenType::EQUAL))
    {
        Token equals = previous();

        auto value = assignment();

        // Assignment is only valid when the
        // left side is a variable.
        if (auto variable = dynamic_cast<Variable*>(left.get()))
        {
            return std::make_unique<Assignment>(
                variable->name,
                std::move(value));
        }

        std::cerr << "Parser Error: Invalid assignment target at line "
                  << equals.line
                  << std::endl;
    }

    return left;
}

std::unique_ptr<Expr> Parser::expression()
/*
  Entry point for expressions.

  The precedence chain is:

      assignment
          ↓
      equality
          ↓
      comparison
          ↓
      term
          ↓
      factor
          ↓
      unary
          ↓
      primary
*/
{
    return assignment();
}

std::unique_ptr<Stmt> Parser::varDeclaration()
/*
  Parses:

      dock age = 20;

  The identifier becomes the variable name.
  The expression after '=' becomes its initializer.
*/
{
    Token name = advance();

    match(TokenType::EQUAL);

    auto initializer = expression();

    match(TokenType::SEMICOLON);

    return std::make_unique<VarStmt>(
        name,
        std::move(initializer));
}

std::unique_ptr<Stmt> Parser::printStatement()
/*
  Parses:

      transmit(age);

  The expression inside the parentheses
  becomes the PrintStmt expression.
*/
{
    match(TokenType::LEFT_PAREN);

    auto value = expression();

    match(TokenType::RIGHT_PAREN);
    match(TokenType::SEMICOLON);

    return std::make_unique<PrintStmt>(
        std::move(value));
}

std::unique_ptr<Stmt> Parser::inputStatement()
/*
  Parses:

      receive(age);

  The identifier represents the variable
  that will receive user input.
*/
{
    // Consume '('
    match(TokenType::LEFT_PAREN);

    // Read the variable name.
    Token name = advance();

    // Consume ')'
    match(TokenType::RIGHT_PAREN);

    // Consume ';'
    match(TokenType::SEMICOLON);

    return std::make_unique<InputStmt>(name);
}

std::unique_ptr<Stmt> Parser::expressionStatement()
/*
  Parses a standalone expression:

      age = 25;

  This is useful for assignments and
  other expressions that are not declarations.
*/
{
    auto value = expression();

    match(TokenType::SEMICOLON);

    return std::make_unique<ExpressionStmt>(
        std::move(value));
}

std::unique_ptr<Stmt> Parser::statement()
/*
  Determines which type of statement
  starts at the current token.
*/
{
    if (match(TokenType::TRANSMIT))
    {
        return printStatement();
    }

    if (match(TokenType::RECEIVE))
    {
        return inputStatement();
    }

    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::declaration()
/*
  Determines whether the current statement
  is a variable declaration or a normal statement.
*/
{
    if (match(TokenType::DOCK))
    {
        return varDeclaration();
    }

    return statement();
}

std::vector<std::unique_ptr<Stmt>> Parser::parse()
/*
  Main entry point of the parser.

  Processes the entire Orbit program statement by statement.
*/
{
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!isAtEnd())
    {
        // Remember where this statement started.
        int start = current;

        statements.push_back(declaration());

        // Safety check:
        // Every successful statement must consume at least one token.
        // Otherwise the parser would loop forever.
        if (current == start)
        {
            std::cerr << "Parser Error: Unexpected token '"
                      << peek().lexeme
                      << "' at line "
                      << peek().line
                      << std::endl;

            advance();
        }
    }

    return statements;
}