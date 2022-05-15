#pragma once
#include "token.hpp"

class Lexer
{
public:
    Lexer(std::string const& src) : m_src(src){}
    
    ~Lexer() = default;

    tokens_t Lex();

    static void PrintTokens(tokens_t const& tokens);

public:
    std::string m_src;
};