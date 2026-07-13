#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

#include "token.h"

class Lexer
{
private:

    std::string source;

    std::vector<Token> tokens;

    int current;

    int line;

public:

    Lexer(const std::string& source);

};

#endif