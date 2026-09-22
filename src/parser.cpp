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
{
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}

std::unique_ptr<Expr> Parser::primary()
{
    // Number literal.
    if (match(TokenType::NUMBER))
    {
        return std::make_unique<Literal>(previous());
    }

    // String literal.
    if (match(TokenType::STRING))
    {
        return std::make_unique<Literal>(previous());
    }

    // Character literal.
    if (match(TokenType::CHAR))
    {
        return std::make_unique<Literal>(previous());
    }

    // Boolean literals.
    if (match(TokenType::TRUE))
    {
        return std::make_unique<Literal>(previous());
    }

    if (match(TokenType::FALSE))
    {
        return std::make_unique<Literal>(previous());
    }

    // Nebula literal.
    //
    // Examples:
    // [1, 2, 3]
    // [[1, 2], [3, 4]]
    // [[[1, 2]], [[3, 4]]]
    //
    // Each element is parsed as a normal expression,
    // so nested arrays work at arbitrary depth.
    if (match(TokenType::LEFT_BRACKET))
    {
        std::vector<std::unique_ptr<Expr>> elements;

        if (!check(TokenType::RIGHT_BRACKET))
        {
            do
            {
                elements.push_back(expression());
            }
            while (match(TokenType::COMMA));
        }

        match(TokenType::RIGHT_BRACKET);

        return std::make_unique<NebulaLiteral>(
            std::move(elements));
    }

    // Grouped expression.
    if (match(TokenType::LEFT_PAREN))
    {
        auto expr = expression();

        match(TokenType::RIGHT_PAREN);

        return std::make_unique<Grouping>(
            std::move(expr));
    }

    // Identifier.
    //
    // A plain identifier starts as a Variable expression.
    // call() will convert a following '(' into a Call expression.
    if (match(TokenType::IDENTIFIER))
    {
        return std::make_unique<Variable>(previous());
    }

    return nullptr;
}

std::unique_ptr<Expr> Parser::finishCall(
    std::unique_ptr<Expr> callee)
{
    std::vector<std::unique_ptr<Expr>> arguments;

    // Parse:
    //
    // ()
    // (10)
    // (10, 20)
    // (x, y + 5, matrix[0])
    //
    // Each argument is a complete Orbit expression.
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            arguments.push_back(expression());
        }
        while (match(TokenType::COMMA));
    }

    // Consume ')'.
    match(TokenType::RIGHT_PAREN);

    return std::make_unique<Call>(
        std::move(callee),
        std::move(arguments));
}

std::unique_ptr<Expr> Parser::call()
{
    auto object = primary();

    if (!object)
    {
        return nullptr;
    }

    // Postfix expressions may contain both calls and
    // chained indexing.
    //
    // Examples:
    //
    // greet()
    // add(10, 20)
    // matrix[0][1]
    // makeMatrix()[0][1]
    //
    // Keep consuming postfix operations until none remain.
    while (true)
    {
        if (match(TokenType::LEFT_PAREN))
        {
            object = finishCall(std::move(object));
        }
        else if (check(TokenType::LEFT_BRACKET))
        {
            object = finishIndexing(std::move(object));
        }
        else
        {
            break;
        }
    }

    return object;
}

std::unique_ptr<Expr> Parser::finishIndexing(
    std::unique_ptr<Expr> object)
{
    // Parse chained indexing:
    //
    // numbers[0]
    // matrix[0][1]
    // cube[1][0][1]
    //
    // Multiple indexing operations are applied from
    // left to right.
    while (match(TokenType::LEFT_BRACKET))
    {
        // Parse the index expression.
        auto index = logicalOr();

        // Consume ']'.
        match(TokenType::RIGHT_BRACKET);

        object = std::make_unique<IndexExpr>(
            std::move(object),
            std::move(index));
    }

    return object;
}

std::unique_ptr<Expr> Parser::unary()
{
    // Unary operators have higher precedence than
    // multiplication, division, modulo, addition
    // and subtraction.
    if (match(TokenType::MINUS) ||
        match(TokenType::BANG))
    {
        Token op = previous();

        auto right = unary();

        return std::make_unique<Unary>(
            op,
            std::move(right));
    }

    return call();
}

std::unique_ptr<Expr> Parser::factor()
{
    // Multiplication, division and modulo have the same
    // precedence and are evaluated from left to right.
    //
    // Example:
    //
    // 20 + 17 % 5 * 2
    //
    // becomes:
    //
    // 20 + ((17 % 5) * 2)
    auto left = unary();

    while (match(TokenType::STAR) ||
           match(TokenType::SLASH) ||
           match(TokenType::MODULO))
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
{
    // Addition and subtraction.
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
{
    auto left = logicalOr();

    if (match(TokenType::EQUAL))
    {
        Token equals = previous();

        // Assignment is right-associative.
        //
        // a = b = 10;
        //
        // becomes:
        //
        // a = (b = 10)
        auto value = assignment();

        // Normal variable assignment.
        if (auto variable =
                dynamic_cast<Variable *>(left.get()))
        {
            return std::make_unique<Assignment>(
                variable->name,
                std::move(value));
        }

        // Indexed assignment.
        //
        // Supports:
        //
        // numbers[1] = 99;
        // matrix[0][1] = 99;
        // cube[1][0][1] = 50;
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
{
    return assignment();
}

std::unique_ptr<Stmt> Parser::varDeclaration()
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
{
    match(TokenType::LEFT_PAREN);

    auto value = expression();

    match(TokenType::RIGHT_PAREN);
    match(TokenType::SEMICOLON);

    return std::make_unique<PrintStmt>(
        std::move(value));
}

std::unique_ptr<Stmt> Parser::inputStatement()
{
    match(TokenType::LEFT_PAREN);

    auto target = expression();

    match(TokenType::RIGHT_PAREN);
    match(TokenType::SEMICOLON);

    return std::make_unique<InputStmt>(
        std::move(target));
}

std::unique_ptr<Stmt> Parser::expressionStatement()
{
    auto value = expression();

    match(TokenType::SEMICOLON);

    return std::make_unique<ExpressionStmt>(
        std::move(value));
}

std::unique_ptr<Stmt> Parser::warpDeclaration()
{
    // Syntax:
    //
    // warp greet() {
    //     transmit("Hello");
    // }
    //
    // warp add(a, b) {
    //     return a + b;
    // }

    Token name = advance();

    // Consume '('.
    match(TokenType::LEFT_PAREN);

    std::vector<Token> params;

    // Parse parameters:
    //
    // warp hello() { ... }
    // warp add(a, b) { ... }
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (!check(TokenType::IDENTIFIER))
            {
                std::cerr
                    << "Parser Error: Expected parameter name at line "
                    << peek().line
                    << std::endl;

                break;
            }

            params.push_back(advance());
        }
        while (match(TokenType::COMMA));
    }

    // Consume ')'.
    match(TokenType::RIGHT_PAREN);

    // Consume '{'.
    match(TokenType::LEFT_BRACE);

    // Parse the complete function body.
    auto body = block();

    return std::make_unique<FunctionStmt>(
        name,
        std::move(params),
        std::move(body));
}

std::unique_ptr<Stmt> Parser::returnStatement()
{
    // return;
    //
    // A return without an expression produces no value.
    if (match(TokenType::SEMICOLON))
    {
        return std::make_unique<ReturnStmt>(nullptr);
    }

    // return expression;
    auto value = expression();

    match(TokenType::SEMICOLON);

    return std::make_unique<ReturnStmt>(
        std::move(value));
}

std::vector<std::unique_ptr<Stmt>> Parser::block()
{
    std::vector<std::unique_ptr<Stmt>> statements;

    // The opening '{' has already been consumed.
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        statements.push_back(declaration());
    }

    // Consume the closing '}'.
    match(TokenType::RIGHT_BRACE);

    return statements;
}

std::unique_ptr<Stmt> Parser::whenStatement()
{
    // Consume '('.
    match(TokenType::LEFT_PAREN);

    auto condition = expression();

    // Consume ')'.
    match(TokenType::RIGHT_PAREN);

    // Consume '{'.
    match(TokenType::LEFT_BRACE);

    auto thenBranch = block();

    std::vector<std::unique_ptr<Stmt>> elseBranch;

    if (match(TokenType::ELSE))
    {
        // Consume '{'.
        match(TokenType::LEFT_BRACE);

        elseBranch = block();
    }

    return std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::orbitingStatement()
{
    // Consume '('.
    match(TokenType::LEFT_PAREN);

    auto condition = expression();

    // Consume ')'.
    match(TokenType::RIGHT_PAREN);

    // Consume '{'.
    match(TokenType::LEFT_BRACE);

    auto body = block();

    return std::make_unique<WhileStmt>(
        std::move(condition),
        std::move(body));
}

std::unique_ptr<Stmt> Parser::statement()
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

    if (match(TokenType::RETURN))
    {
        return returnStatement();
    }

    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::declaration()
{
    if (match(TokenType::DOCK))
    {
        return varDeclaration();
    }

    if (match(TokenType::NEBULA))
    {
        return nebulaDeclaration();
    }

    if (match(TokenType::WARP))
    {
        return warpDeclaration();
    }

    return statement();
}

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!isAtEnd())
    {
        // Remember where this statement started.
        int start = current;

        statements.push_back(declaration());

        // Safety check:
        // Every successful statement must consume at least one token.
        // Otherwise the parser could loop forever.
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