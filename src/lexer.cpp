#include "../include/lexer.h"
#include <iostream>

Lexer::Lexer(const std::string &source)
{
    this->source = source;

    this->current = 0;

    this->line = 1;

    keywords["dock"] = TokenType::DOCK;
    keywords["transmit"] = TokenType::TRANSMIT;
    keywords["receive"] = TokenType::RECEIVE;
    keywords["true"] = TokenType::TRUE;
    keywords["false"] = TokenType::FALSE;
    keywords["when"] = TokenType::WHEN;
    keywords["else"] = TokenType::ELSE;
    keywords["orbiting"] = TokenType::ORBITING;
    keywords["nebula"] = TokenType::NEBULA;
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

bool Lexer::isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

void Lexer::identifier()
{
    std::string word;

    while (isAlphaNumeric(peek()))
    {
        word += advance();
    }

    if (word == "comet" && peek() == ':')
    {
        advance();

        skipComment();

        return;
    }

    if (keywords.find(word) != keywords.end())
    {
        tokens.push_back(
            Token(keywords[word], word, line));
    }
    else
    {
        tokens.push_back(
            Token(TokenType::IDENTIFIER, word, line));
    }
}

void Lexer::scanToken()
{
    char c = advance();

    switch (c)
    {
    case '+':
        tokens.push_back(
            Token(TokenType::PLUS, "+", line));
        break;

    case '-':
        tokens.push_back(
            Token(TokenType::MINUS, "-", line));
        break;

    case '*':
        tokens.push_back(
            Token(TokenType::STAR, "*", line));
        break;

    case '/':
        tokens.push_back(
            Token(TokenType::SLASH, "/", line));
        break;

    case '=':

        if (peek() == '=')
        {
            advance();

            tokens.push_back(
                Token(TokenType::EQUAL_EQUAL, "==", line));
        }
        else
        {
            tokens.push_back(
                Token(TokenType::EQUAL, "=", line));
        }

        break;

    case '<':

        if (peek() == '=')
        {
            advance();

            tokens.push_back(
                Token(TokenType::LESS_EQUAL, "<=", line));
        }
        else
        {
            tokens.push_back(
                Token(TokenType::LESS, "<", line));
        }

        break;

    case '>':
        if (peek() == '=')
        {
            advance();

            tokens.push_back(
                Token(TokenType::GREATER_EQUAL, ">=", line));
        }
        else
        {
            tokens.push_back(
                Token(TokenType::GREATER, ">", line));
        }

        break;

    case '!':
        if (peek() == '=')
        {
            advance();

            tokens.push_back(
                Token(TokenType::NOT_EQUAL, "!=", line));
        }
        else
        {
            tokens.push_back(
                Token(TokenType::BANG, "!", line));
        }

        break;

    case '&':
        if (peek() == '&')
        {
            advance();

            tokens.push_back(
                Token(TokenType::AND, "&&", line));
        }
        else
        {
            std::cerr
                << "Lexer Error: Expected '&' after '&' at line "
                << line
                << std::endl;
        }

        break;

    case '|':
        if (peek() == '|')
        {
            advance();

            tokens.push_back(
                Token(TokenType::OR, "||", line));
        }
        else
        {
            std::cerr
                << "Lexer Error: Expected '|' after '|' at line "
                << line
                << std::endl;
        }

        break;

    case ';':
        tokens.push_back(
            Token(TokenType::SEMICOLON, ";", line));
        break;

    case '(':
        tokens.push_back(
            Token(TokenType::LEFT_PAREN, "(", line));
        break;

    case ')':
        tokens.push_back(
            Token(TokenType::RIGHT_PAREN, ")", line));
        break;

    case '{':
        tokens.push_back(
            Token(TokenType::LEFT_BRACE, "{", line));
        break;

    case '}':
        tokens.push_back(
            Token(TokenType::RIGHT_BRACE, "}", line));
        break;

    case '[':
        tokens.push_back(
            Token(TokenType::LEFT_BRACKET, "[", line));
        break;

    case ']':
        tokens.push_back(
            Token(TokenType::RIGHT_BRACKET, "]", line));
        break;

    case ',':
        tokens.push_back(
            Token(TokenType::COMMA, ",", line));
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

    case '\'':
        charLiteral();
        break;

    default:

        if (isAlpha(c))
        {
            current--;
            identifier();
        }

        else if (isDigit(c))
        {
            current--;
            number();
        }

        else
        {
            std::cerr
                << "Lexer Error: Unexpected character '"
                << c
                << "' at line "
                << line
                << std::endl;
        }

        break;
    }
}

std::vector<Token> Lexer::scanTokens()
{
    while (!isAtEnd())
    {
        scanToken();
    }

    tokens.push_back(
        Token(TokenType::END_OF_FILE, "", line));

    return tokens;
}

bool Lexer::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

void Lexer::number()
{
    std::string value;

    while (isDigit(peek()))
    {
        value += advance();
    }

    tokens.push_back(
        Token(TokenType::NUMBER, value, line));
}

bool Lexer::isAlphaNumeric(char c)
{
    return isAlpha(c) || isDigit(c);
}

void Lexer::stringLiteral()
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
        std::cerr
            << "Lexer Error: Unterminated string at line "
            << line
            << std::endl;

        return;
    }

    // Consume closing double quote.
    advance();

    tokens.push_back(
        Token(TokenType::STRING, value, line));
}

void Lexer::charLiteral()
{
    // A character literal must contain exactly
    // one character between single quotes.
    //
    // Example:
    // 'A'
    //
    // The quotes themselves are not stored in
    // the token lexeme.

    if (isAtEnd() || peek() == '\n')
    {
        std::cerr
            << "Lexer Error: Unterminated character at line "
            << line
            << std::endl;

        return;
    }

    char value = advance();

    // A character literal must contain exactly
    // one character.
    if (peek() != '\'')
    {
        std::cerr
            << "Lexer Error: Character literal must contain exactly one character at line "
            << line
            << std::endl;

        // Skip the remaining invalid character literal.
        while (!isAtEnd() &&
               peek() != '\'' &&
               peek() != '\n')
        {
            advance();
        }

        // Consume the closing quote if one exists.
        if (!isAtEnd() && peek() == '\'')
        {
            advance();
        }

        return;
    }

    // Consume closing single quote.
    advance();

    tokens.push_back(
        Token(
            TokenType::CHAR,
            std::string(1, value),
            line));
}

void Lexer::skipComment()
{
    while (!isAtEnd())
    {
        // Skip spaces and other whitespace before the next word.
        while (!isAtEnd() &&
               (peek() == ' ' ||
                peek() == '\t' ||
                peek() == '\r'))
        {
            advance();
        }

        // Read the next word.
        std::string word;

        while (!isAtEnd() && isAlphaNumeric(peek()))
        {
            word += advance();
        }

        // If the word is "burn", the comment is finished.
        if (word == "burn")
        {
            return;
        }

        // Ignore other characters inside the comment.
        if (!isAtEnd() && peek() != '\n')
        {
            advance();
        }

        // If the comment reaches a new line without "burn",
        // report an unterminated comment.
        if (!isAtEnd() && peek() == '\n')
        {
            std::cerr
                << "Lexer Error: Unterminated comment at line "
                << line
                << std::endl;

            advance();
            line++;

            return;
        }
    }

    // Reached the end of the source without finding "burn".
    std::cerr
        << "Lexer Error: Unterminated comment at line "
        << line
        << std::endl;
}