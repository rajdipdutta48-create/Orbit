#include "../include/lexer.h"

Lexer::Lexer(const std::string& source)
{
    this->source = source;

    this->current = 0;

    this->line = 1;
}
bool Lexer::isAtEnd()
{
    return current >= source.length();
}
char Lexer::peek()
{
    if (isAtEnd())
        return '\0';

    return source[current];
}
char Lexer::advance()
{
    return source[current++];
}