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

    char advance();

    char peek();

    bool isAlpha(char c);

    void identifier();

    void scanToken();

public:
   
    Lexer(const std::string& source);

    std::vector<Token> scanTokens();

};

#endif