#include "../include/lexer.h"

Lexer::Lexer(const std::string& source)
{
    this->source = source;

    this->current = 0;

    this->line = 1;

    keywords["let"] = TokenType::LET;
    keywords["print"] = TokenType::PRINT;
}

bool Lexer::isAtEnd()//tells us if the current has reached the end or not.
{
    return current >= source.length();
}

char Lexer::peek()//returns the character of source which is at current position.
{
    if (isAtEnd())
        return '\0';

    return source[current];
}

char Lexer::advance()//advances to the next iteration returning the current element.
{
    return source[current++];
}

bool Lexer::isAlpha(char c)//tells us if the character is an alphabet or not.
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

void Lexer::identifier()// Reads a complete identifier or keyword and creates the corresponding token.
{
    std::string word;

    while (isAlphaNumeric(peek()))
    {
        word += advance();
    }
     if (word == "comet" && peek() == ':')//skip
    {
    advance();

    skipComment();

    return;
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

void Lexer::scanToken()// Processes one lexical unit from the source code and creates the appropriate token.
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
        
        case ' ':
        case '\t':
        case '\r':
            break;

        case '\n':
             line++;
            break;

        case '"':
            stringLiteral();
            break;
        
        default:

            if(isAlpha(c))
            {
                current--;
                identifier();
            }

            else if (isDigit(c))
            {
                current--;
                number();
            }

            break;
    }
}

std::vector<Token> Lexer::scanTokens()//used to fill the tokens vector.
{
    while (!isAtEnd())
    {
        scanToken();
    }

    return tokens;
}

bool Lexer::isDigit(char c)//checks if the character is a number.
{
    return c >= '0' && c <= '9';
}

void Lexer::number()// Reads a complete numeric literal and creates a NUMBER token.
{
    std::string value;

    while (isDigit(peek()))
    {
        value += advance();
    }

    tokens.push_back(
        Token(TokenType::NUMBER, value, line)
    );
}

bool Lexer::isAlphaNumeric(char c)//checks if the character is either numeric or alphabet.
{
    return isAlpha(c) || isDigit(c);
}

void Lexer::stringLiteral()//generate string type tokens.
{
    std::string value;

    while (!isAtEnd() && peek() != '"')
    {
        if (peek() == '\n')
        {
            line++;
        }

        value += advance();
    }

    if (isAtEnd())
    {
        return;
    }

    advance();

    tokens.push_back(
        Token(TokenType::STRING, value, line)
    );
}

void Lexer::skipComment()//ignores comments inside code
{
    while (!isAtEnd())
    {
        std::string word;

        while (!isAtEnd() && peek() != '\n')
        {
            word += advance();
        }

        if (word == "burn")
        {
            return;
        }

        if (!isAtEnd())
        {
            line++;
            advance();
        }
    }
}