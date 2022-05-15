#include "lexer.hpp"

std::string ReadString(std::string const& src, unsigned index)
{
    std::string buffer;
    char c = 0;
    ++index; // skip first quote
    while(true){
        char const& c = src[index++];
        if(c == '"' || c == 0) break;

        buffer.push_back(c);
    }
    return buffer;
}
#include <stdexcept>

std::string ReadNumber(std::string const& src, unsigned& index)
{

    size_t size = 0;
    std::stold(src.substr(index), &size);
    std::string str = src.substr(index, size);
    index += size;

    return str;
}

Token LexIdentifier(std::string const& src, unsigned& index, int line)
{
    std::string value;
    while(isalnum(src[index]))
        value.push_back(src[index++]);

    if(value == "null")
        return Token(TokenType::_NULL, line, index);
    if(value == "true")
        return Token(TokenType::TRUE, line, index);
    if(value == "false")
        return Token(TokenType::FALSE, line, index);
    throw std::runtime_error("[LexIdentifier]: invalid identifier");
}


tokens_t Lexer::Lex()
{
    tokens_t tokens;

    unsigned line = 1;

    unsigned index = 0;
    while(index < m_src.length())
    {
        char c = m_src[index];
        if(c == '"')
        {
            std::string value = ReadString(m_src, index);
            tokens.push_back(Token(TokenType::STRING, line,index, value));
            index += value.length() + 2; continue;
        }
        if(isdigit(c) || c == '-')
        {
            std::string value = ReadNumber(m_src, index);
            tokens.push_back(Token(TokenType::NUMBER, line, index, value));
            continue;
        }
        if(isalnum(c))
        {
            Token token = LexIdentifier(m_src, index, line);
            tokens.push_back(token);
            continue;
        }
        if(c == '\n')
            ++line;

        if(c == '[')
            tokens.push_back(Token(TokenType::SQUARE_OPEN,line, index));
        if(c == ']')
            tokens.push_back(Token(TokenType::SQUARE_CLOSE,line, index));
        if(c == '{')
            tokens.push_back(Token(TokenType::BRACKET_OPEN,line, index));
        if(c == '}')
            tokens.push_back(Token(TokenType::BRACKET_CLOSE,line, index));
        if(c == ':')
            tokens.push_back(Token(TokenType::COLON,line, index));
        if(c == ',') 
            tokens.push_back(Token(TokenType::COMMA,line, index));
        ++index;
    }

    return tokens;
}

void Lexer::PrintTokens(tokens_t const& tokens)
{
    for(auto& token : tokens)
    {
        printf("Token type [%s] value [%s]\n", TokenStrings[(int)token.type].c_str(), token.value.c_str());
    }
}
