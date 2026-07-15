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

    // Symbols
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
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