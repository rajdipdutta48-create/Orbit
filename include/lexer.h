#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>

#include "token.h"

class Lexer
{
private:
    std::string source;

    std::vector<Token> tokens;

    std::unordered_map<std::string, TokenType> keywords;

    int current;

    int line;

    bool isAtEnd();

    char peek();

    char advance();

    bool isAlpha(char c);

    bool isDigit(char c);

    bool isAlphaNumeric(char c);

    void identifier();

    void number();

    // Reads a string enclosed by double quotes.
    //
    // Example:
    // "Orbit"
    void stringLiteral();

    // Reads a single character enclosed by single quotes.
    //
    // Example:
    // 'A'
    void charLiteral();

    // Reads:
    //
    // comet: anything burn
    //
    // and ignores everything between comet: and burn.
    void skipComment();

    void scanToken();

public:
    Lexer(const std::string &source);

    std::vector<Token> scanTokens();
};

#endif