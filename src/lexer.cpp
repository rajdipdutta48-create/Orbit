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

    // Function-related keywords.
    keywords["warp"] = TokenType::WARP;
    keywords["return"] = TokenType::RETURN;
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

    // Orbit comments use:
    //
    // comet: comment text burn
    //
    // The entire comment is ignored.
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

    case '%':
        tokens.push_back(
            Token(TokenType::MODULO, "%", line));
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

    // Read the integer part.
    while (isDigit(peek()))
    {
        value += advance();
    }

    // Read the decimal part if a dot is
    // followed by at least one digit.
    //
    // Examples:
    // 25.5
    // 99.99
    // 0.5
    if (peek() == '.' &&
        current + 1 < source.length() &&
        isDigit(source[current + 1]))
    {
        value += advance();

        while (isDigit(peek()))
        {
            value += advance();
        }
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
    // Orbit comments can span multiple lines.
    //
    // Example:
    //
    // comet:
    // This program stores an age
    // The comment helps explain the code
    // burn
    //
    // Everything between "comet:" and the standalone
    // word "burn" is ignored by the lexer.

    while (!isAtEnd())
    {
        // Preserve correct source line numbers.
        if (peek() == '\n')
        {
            advance();
            line++;
            continue;
        }

        // Read complete words so that:
        //
        // burn     -> ends the comment
        // burned   -> remains comment text
        //
        if (isAlpha(peek()))
        {
            std::string word;

            while (!isAtEnd() && isAlphaNumeric(peek()))
            {
                word += advance();
            }

            if (word == "burn")
            {
                return;
            }

            continue;
        }

        // Ignore spaces, punctuation, numbers and
        // every other character inside the comment.
        advance();
    }

    // Reached EOF without finding "burn".
    std::cerr
        << "Lexer Error: Unterminated comment at line "
        << line
        << std::endl;
}