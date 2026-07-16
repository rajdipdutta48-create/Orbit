#include "../include/parser.h"

Parser::Parser(const std::vector<Token> &tokens)
{
  this->tokens = tokens;
  current = 0;
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

bool Parser::match(TokenType type) /* If the current token matches the expected type,consume it by advancing and return true.Otherwise, leave the parser unchanged and return false.*/
{
  if (check(type))
  {
    advance();
    return true;
  }

  return false;
}

std::unique_ptr<Expr> Parser::primary() /*Parses the simplest expression (currently only literals)
  and creates the corresponding AST node.*/
{
  if (match(TokenType::NUMBER))
  {
    return std::make_unique<Literal>(previous());
  }

  return nullptr;
}

std::unique_ptr<Expr> Parser::expression() /*Parses a simple binary expression containing '+'.
  Creates a Binary AST node from two primary expressions.*/
{
  auto left = primary();

  while (match(TokenType::PLUS))
  {
    Token op = previous();

    auto right = primary();

    left = std::make_unique<Binary>(
        std::move(left),
        op,
        std::move(right));
  }

  return left;
}

std::unique_ptr<Expr> Parser::parse() /*Entry point of the parser.
 Starts parsing from the highest grammar rule.*/
{
  return expression();
}