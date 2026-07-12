#include "../include/token.h"

Token::Token(TokenType type, const std::string& lexeme, int line)
{
    this->type = type;
    this->lexeme = lexeme;
    this->line = line;
}