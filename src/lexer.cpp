#include "../include/lexer.h"

Lexer::Lexer(const std::string& source)
{
    this->source = source;

    this->current = 0;

    this->line = 1;

    keywords["let"] = TokenType::LET;
    keywords["print"] = TokenType::PRINT;
}

bool Lexer::isAtEnd()//tells us if the current has reached the end or not
{
    return current >= source.length();
}

char Lexer::peek()//returns the character of source which is at current position
{
    if (isAtEnd())
        return '\0';

    return source[current];
}

char Lexer::advance()//advances to the next iteration returning the current element
{
    return source[current++];
}

bool Lexer::isAlpha(char c)//tells us if the character is an alphabet or not
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

void Lexer::identifier()//lexer making the sequence of tokens
{
    std::string word;

    while (isAlpha(peek()))
    {
        word += advance();
    }

    if (keywords.find(word) != keywords.end())
    {
        tokens.push_back(
            Token(keywords[word], word, line)
        );
    }
    else
    {
        tokens.push_back(
            Token(TokenType::IDENTIFIER, word, line)
        );
    }
}

void Lexer::scanToken()//Returns the current character without advancing the reading position.
{
    char c = advance();

    switch(c)
    {
        case '+':
            tokens.push_back(
                Token(TokenType::PLUS, "+", line)
            );
            break;

        case '-':
            tokens.push_back(
                Token(TokenType::MINUS, "-", line)
            );
            break;

        case '*':
            tokens.push_back(
                Token(TokenType::STAR, "*", line)
            );
            break;

        case '/':
            tokens.push_back(
                Token(TokenType::SLASH, "/", line)
            );
            break;

        case '=':
            tokens.push_back(
                Token(TokenType::EQUAL, "=", line)
            );
            break;

        case ';':
            tokens.push_back(
                Token(TokenType::SEMICOLON, ";", line)
            );
            break;

        case '(':
            tokens.push_back(
                Token(TokenType::LEFT_PAREN, "(", line)
            );
            break;

        case ')':
            tokens.push_back(
                Token(TokenType::RIGHT_PAREN, ")", line)
            );
            break;

        default:

            if(isAlpha(c))
            {
                current--;
                identifier();
            }

            break;
    }
}