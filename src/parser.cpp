#include "../include/parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token> &tokens)
    : tokens(tokens), current(0)
{
}

Token Parser::peek()
{
    return tokens[current];
}

Token Parser::previous()
{
    return tokens[current - 1];
}

bool Parser::isAtEnd()
{
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::advance()
{
    if (!isAtEnd())
    {
        current++;
    }

    return previous();
}

bool Parser::check(TokenType type)
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
  - String literals
  - Character literals
  - Boolean literals
  - Parenthesized expressions
  - Variables / identifiers
  - Nebula literals
*/
{
    // Example: 123
    if (match(TokenType::NUMBER))
    {
        return std::make_unique<Literal>(previous());
    }

    // Example: "Orbit"
    if (match(TokenType::STRING))
    {
        return std::make_unique<Literal>(previous());
    }

    // Example: 'O'
    if (match(TokenType::CHAR))
    {
        return std::make_unique<Literal>(previous());
    }

    // Example: true
    if (match(TokenType::TRUE))
    {
        return std::make_unique<Literal>(previous());
    }

    // Example: false
    if (match(TokenType::FALSE))
    {
        return std::make_unique<Literal>(previous());
    }

    // Examples:
    //
    // [10, 20, 30]
    // ["Earth", "Mars"]
    // ['A', 'B', 'C']
    //
    // Nested nebula literals are also supported:
    //
    // [[1, 2], [3, 4]]
    //
    // Since every element is parsed as a normal expression,
    // a nebula can contain another nebula at any depth.
    if (match(TokenType::LEFT_BRACKET))
    {
        std::vector<std::unique_ptr<Expr>> elements;

        // Empty nebula:
        // []
        if (!check(TokenType::RIGHT_BRACKET))
        {
            do
            {
                elements.push_back(expression());
            }
            while (match(TokenType::COMMA));
        }

        // Consume ']'
        match(TokenType::RIGHT_BRACKET);

        return std::make_unique<NebulaLiteral>(
            std::move(elements));
    }

    // Example: (10 + 20)
    if (match(TokenType::LEFT_PAREN))
    {
        auto expr = expression();

        match(TokenType::RIGHT_PAREN);

        return std::make_unique<Grouping>(
            std::move(expr));
    }

    // Example: age
    if (match(TokenType::IDENTIFIER))
    {
        return std::make_unique<Variable>(previous());
    }

    // No valid primary expression was found.
    return nullptr;
}

std::unique_ptr<Expr> Parser::finishIndexing(
    std::unique_ptr<Expr> object)
/*
  Parses array indexing after an expression.

  Examples:

      numbers[0]

      matrix[0][1]

      cube[1][0][1]

  The expression before '[' becomes the object.

  Because this is handled inside a loop, indexing can be
  chained to any depth:

      a[0]
      a[0][1]
      a[0][1][2]
      a[0][1][2][3]

  Each additional '[index]' wraps the previous expression
  inside another IndexExpr.
*/
{
    while (match(TokenType::LEFT_BRACKET))
    {
        // Parse the index without allowing an assignment
        // inside the brackets.
        auto index = logicalOr();

        // Consume ']'
        match(TokenType::RIGHT_BRACKET);

        object = std::make_unique<IndexExpr>(
            std::move(object),
            std::move(index));
    }

    return object;
}

std::unique_ptr<Expr> Parser::unary()
/*
  Parses unary expressions.

  Examples:
      -10
      -age
      -(10 + 5)
      !true
      !age

  Unary operators have higher precedence than
  multiplication, division, addition and subtraction.
*/
{
    if (match(TokenType::MINUS) ||
        match(TokenType::BANG))
    {
        Token op = previous();

        auto right = unary();

        return std::make_unique<Unary>(
            op,
            std::move(right));
    }

    // Parse the primary expression first,
    // then check whether it is being indexed.
    auto object = primary();

    return finishIndexing(std::move(object));
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

    while (match(TokenType::STAR) ||
           match(TokenType::SLASH))
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

    while (match(TokenType::PLUS) ||
           match(TokenType::MINUS))
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

std::unique_ptr<Expr> Parser::logicalAnd()
/*
  Parses logical AND expressions.

  Example:

      true && false

  AND has higher precedence than OR.
*/
{
    auto left = equality();

    while (match(TokenType::AND))
    {
        Token op = previous();

        auto right = equality();

        left = std::make_unique<Binary>(
            std::move(left),
            op,
            std::move(right));
    }

    return left;
}

std::unique_ptr<Expr> Parser::logicalOr()
/*
  Parses logical OR expressions.

  Example:

      true || false

  OR has lower precedence than AND.
*/
{
    auto left = logicalAnd();

    while (match(TokenType::OR))
    {
        Token op = previous();

        auto right = logicalAnd();

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

  Examples:

      age = 25;

      numbers[1] = 99;

      matrix[0][1] = 99;

      cube[1][0][1] = 50;

  Assignment has lower precedence than
  logical, equality and comparison expressions.

  The left side must be either:
  - a variable
  - an indexed array element

  Indexed assignment supports arbitrary nesting because
  an IndexExpr can itself contain another IndexExpr.

  Example:

      matrix[0][1] = 99;

  is represented conceptually as:

      IndexAssignment(
          object = IndexExpr(
              object = matrix,
              index = 0
          ),
          index = 1,
          value = 99
      )
*/
{
    auto left = logicalOr();

    if (match(TokenType::EQUAL))
    {
        Token equals = previous();

        auto value = assignment();

        // Assignment to a normal variable.
        if (auto variable =
                dynamic_cast<Variable *>(left.get()))
        {
            return std::make_unique<Assignment>(
                variable->name,
                std::move(value));
        }

        // Assignment to an array element.
        //
        // Examples:
        //
        // numbers[1] = 99;
        //
        // matrix[0][1] = 99;
        //
        // cube[1][0][1] = 50;
        //
        // For nested indexing, indexExpr->object can itself
        // be another IndexExpr. Moving it into IndexAssignment
        // preserves the complete indexing chain.
        if (auto indexExpr =
                dynamic_cast<IndexExpr *>(left.get()))
        {
            auto object = std::move(indexExpr->object);
            auto index = std::move(indexExpr->index);

            return std::make_unique<IndexAssignment>(
                std::move(object),
                std::move(index),
                std::move(value));
        }

        std::cerr
            << "Parser Error: Invalid assignment target at line "
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
      logicalOr
          ↓
      logicalAnd
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

std::unique_ptr<Stmt> Parser::nebulaDeclaration()
/*
  Parses:

      nebula numbers = [10, 20, 30];

  The identifier becomes the nebula name.
  The expression after '=' becomes its initializer.

  Nested nebula literals are supported because the
  initializer is parsed as a normal expression.
*/
{
    Token name = advance();

    match(TokenType::EQUAL);

    auto initializer = expression();

    match(TokenType::SEMICOLON);

    return std::make_unique<NebulaStmt>(
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
      receive(numbers[1]);
      receive(matrix[0][1]);

  The target can be:
  - a normal variable
  - an indexed nebula element
  - a nested indexed nebula element
*/
{
    // Consume '('
    match(TokenType::LEFT_PAREN);

    // Parse the target expression.
    auto target = expression();

    // Consume ')'
    match(TokenType::RIGHT_PAREN);

    // Consume ';'
    match(TokenType::SEMICOLON);

    return std::make_unique<InputStmt>(
        std::move(target));
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

std::vector<std::unique_ptr<Stmt>> Parser::block()
/*
  Parses a block of statements inside:

      {
          ...
      }

  The opening '{' must already have been consumed.
*/
{
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        statements.push_back(declaration());
    }

    // Consume the closing '}'
    match(TokenType::RIGHT_BRACE);

    return statements;
}

std::unique_ptr<Stmt> Parser::whenStatement()
/*
  Parses:

      when (condition) {
          statements
      }

  Optional else:

      when (condition) {
          statements
      } else {
          statements
      }
*/
{
    // Consume '('
    match(TokenType::LEFT_PAREN);

    // Parse the condition.
    auto condition = expression();

    // Consume ')'
    match(TokenType::RIGHT_PAREN);

    // Consume '{'
    match(TokenType::LEFT_BRACE);

    // Parse the statements inside the when block.
    auto thenBranch = block();

    std::vector<std::unique_ptr<Stmt>> elseBranch;

    // Check whether an else block exists.
    if (match(TokenType::ELSE))
    {
        // Consume '{'
        match(TokenType::LEFT_BRACE);

        // Parse the statements inside the else block.
        elseBranch = block();
    }

    return std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::orbitingStatement()
/*
  Parses:

      orbiting (condition) {
          statements
      }

  The condition is evaluated before every
  iteration of the loop.
*/
{
    // Consume '('
    match(TokenType::LEFT_PAREN);

    // Parse the loop condition.
    auto condition = expression();

    // Consume ')'
    match(TokenType::RIGHT_PAREN);

    // Consume '{'
    match(TokenType::LEFT_BRACE);

    // Parse the statements inside the loop.
    auto body = block();

    return std::make_unique<WhileStmt>(
        std::move(condition),
        std::move(body));
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

    if (match(TokenType::WHEN))
    {
        return whenStatement();
    }

    if (match(TokenType::ORBITING))
    {
        return orbitingStatement();
    }

    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::declaration()
/*
  Determines whether the current statement
  is a variable declaration, nebula declaration,
  or a normal statement.
*/
{
    if (match(TokenType::DOCK))
    {
        return varDeclaration();
    }

    if (match(TokenType::NEBULA))
    {
        return nebulaDeclaration();
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
            std::cerr
                << "Parser Error: Unexpected token '"
                << peek().lexeme
                << "' at line "
                << peek().line
                << std::endl;

            advance();
        }
    }

    return statements;
}