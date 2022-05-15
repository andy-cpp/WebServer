#pragma once

#include <vector>
#include <cstdint>
#include <string>
#undef NULL
enum class TokenType{
    BRACKET_OPEN,
    BRACKET_CLOSE,
    SQUARE_OPEN,
    SQUARE_CLOSE,
    STRING,
    NUMBER,
    TRUE,
    FALSE,
    _NULL,
    COLON,
    COMMA
};

inline std::string TokenStrings[] = {
    "BRACKET_OPEN",
    "BRACKET_CLOSE",
    "SQUARE_OPEN",
    "SQUARE_CLOSE",
    "STRING",
    "NUMBER",
    "TRUE",
    "FALSE",
    "NULL",
    "COLON",
    "COMMA",
};

struct Token{

    Token(TokenType type, int line, unsigned offset, std::string value = "") : type(type), value(value), line(line), offset(offset){}

    TokenType type;
    std::string value;
    int line, offset;
};

typedef Token token_t;
typedef std::vector<Token> tokens_t;