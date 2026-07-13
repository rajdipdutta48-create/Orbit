#include "../include/lexer.h"

Lexer::Lexer(const std::string& source)
{
    this->source = source;

    this->current = 0;

    this->line = 1;
}