#include "../include/parser.h"

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

  Unary operators will be added later.
  For now, it simply passes control to primary().
*/
{
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

std::unique_ptr<Expr> Parser::expression()
/*
  Entry point for expressions.

  Currently expression() delegates to term().
  More grammar levels such as comparison and equality
  can be added above term() later.
*/
{
    return term();
}

std::unique_ptr<Expr> Parser::parse()
/*
  Main entry point of the parser.

  Starts parsing from the highest expression grammar rule.
*/
{
    return expression();
}