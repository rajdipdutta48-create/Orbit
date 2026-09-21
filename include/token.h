#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// Every possible type of token in Orbit
enum class TokenType
{
    // Keywords
    DOCK,
    TRANSMIT,
    RECEIVE,
    TRUE,
    FALSE,
    WHEN,
    ELSE,
    ORBITING,
    NEBULA,

    // Identifiers and literals
    IDENTIFIER,
    NUMBER,
    STRING,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    EQUAL,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    BANG,
    AND,
    OR,

    // Symbols
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COMMA,
    SEMICOLON,

    // Special token
    END_OF_FILE
};

class Token
{
public:
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, const std::string &lexeme, int line);
};

#endif