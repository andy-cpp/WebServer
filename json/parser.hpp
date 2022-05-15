#pragma once

#include "lexer.hpp"
#include "Object.hpp"

class Parser
{
public:
    Parser(std::string const& src) : m_src(src){tokens = Lexer(src).Lex();};

    ~Parser() = default;

    Object Parse();

    void ReadAttribute(unsigned& index);

private:
    Object ParseArray(unsigned& index);

    Object ParseObject(unsigned& index);

    Object ParseValue(Token& token, unsigned& index);

private:


public:
    tokens_t tokens;

    std::string m_src;
};