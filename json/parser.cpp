#include "parser.hpp"
#include <stdexcept>
#include <assert.h>

#define EXPECT_EQ(type_, offset) (tokens[index + offset].type == type_)


void Parser::ReadAttribute(unsigned& index)
{
    std::string key = tokens[index].value;
    if(!EXPECT_EQ(TokenType::COLON, 1)) printf("isnt colon :(\n");
    std::string value = tokens[index + 2].value;
    printf("\"%s\": \"%s\"\n", key.c_str(), value.c_str());

    index += 3;
}

inline bool IsValueType(TokenType type)
{
    return (type == TokenType::STRING || type == TokenType::NUMBER || type == TokenType::SQUARE_OPEN || type == TokenType::BRACKET_OPEN || type == TokenType::_NULL || type == TokenType::FALSE || type == TokenType::TRUE);
}

bool LastTokenIsType(Token* LastToken, TokenType type)
{
    return (LastToken) ? LastToken->type == type : false;
}

Object Parser::ParseValue(Token& token, unsigned& index)
{

    //printf("Parsing type: %s\ttoken.offset: %d\n", TokenStrings[(int)token.type].c_str(), token.offset);
    if(!IsValueType(token.type))
        throw std::runtime_error(std::string("[Parser::ParseValue]: expected value, got ") + TokenStrings[(int)token.type]);
            switch(token.type)
            {
            case TokenType::STRING:
                //printf("returning %s\n", token.value.c_str());
                return token.value;
                break;
            case TokenType::NUMBER:
                return Object(std::stold(token.value));
                break;
            case TokenType::BRACKET_OPEN:
                return ParseObject(index);
                break;
            case TokenType::SQUARE_OPEN:
                return ParseArray(index);
                break;
            case TokenType::_NULL:
                return Object();
            case TokenType::TRUE: {
                Object obj(true);
                obj.m_type = Object::Type::BOOL;
                return obj;
            }
            case TokenType::FALSE: {
                Object obj(false);
                obj.m_type = Object::Type::BOOL;
                return obj;
            }
            default:
                throw std::runtime_error("[Parser::ParseArray]: unknown value passed");
            }


    throw std::runtime_error("[Parser::ParseValue]: could not parse value");
}

Object Parser::ParseArray(unsigned& index)
{
    Object::array_t array;
    index += 1; // skip '[' token
    Token* LastToken = 0;
    bool ExpectValue = true;
    bool ExpectComma = false;

    for(; index < tokens.size(); ++index)
    {
        Token& token = tokens[index];

        if(token.type == TokenType::SQUARE_CLOSE && !LastTokenIsType(LastToken, TokenType::COMMA))
            { ++index;break;}

        if(ExpectValue)
        {
            array.push_back(ParseValue(token, index));
            ExpectComma = true;
            ExpectValue = false;
            goto end;

        }
        if(ExpectComma && token.type != TokenType::COMMA && token.type != TokenType::SQUARE_CLOSE){
            throw std::runtime_error(std::string("[Parser::ParseArray]: expected comma, got ") + TokenStrings[(int)token.type]);
        }
        else {
            ExpectComma = false;
            ExpectValue = true;
        }
        end:
        LastToken = &token;
        
    }
    --index;
    //;
    //printf("index when returning array: %d\n", index);
    return Object(array);
}

Object Parser::ParseObject(unsigned& index)
{
    Object::dictionary_t obj;
    index += 1;
    bool Key = true;

    bool ExpectKey = true;
    bool ExpectColon = false;
    bool ExpectValue = false;
    bool ExpectComma = false;
    std::string key, value;
    Token* LastToken = 0;

    for(;index < tokens.size(); ++index)
    {
        Token& token = tokens[index];
        //printf("token.type: %d\n", token.type);
        //printf("i: %d\ttoken.type: %d\tExpectKey: %d\tExpectColon: %d\tExpectValue: %d\tExpectComma: %d\n", index,token.type, ExpectKey, ExpectColon, ExpectValue, ExpectComma);

        //if(token.type == TokenType::BRACKET_CLOSE){ ++index; break;};

        if(token.type == TokenType::BRACKET_CLOSE && !LastTokenIsType(LastToken, TokenType::COMMA))
            { ++index;break;}

        if(ExpectKey){
            if(token.type != TokenType::STRING)
                throw std::runtime_error(std::string("[Parser::ParseObject]: expected key, got ") + TokenStrings[(int)token.type]);
            key = token.value;
            ExpectKey = false;
            ExpectColon = true;

            goto end;
        }
        if(ExpectColon){
            if(token.type != TokenType::COLON)
                throw std::runtime_error("[Parser::ParseObject]: expected colon");

            ExpectColon = false;
            ExpectValue = true;
            //printf("Expecting value!\n");
            goto end;
        }
        if(ExpectValue)
        {
            if(!IsValueType(token.type))
                throw std::runtime_error("[Parser::ParseObject]: invalid value type");

                //printf("type : %s\n", TokenStrings[(int)token.type].c_str());
                obj[key] = ParseValue(token, index);
                ExpectValue = false;
                ExpectComma = true;
                goto end;
        }

        if(ExpectComma && token.type != TokenType::COMMA && token.type != TokenType::BRACKET_CLOSE && token.type != TokenType::SQUARE_CLOSE){
                    //printf("i: %d\ttoken.type: %d\tExpectKey: %d\tExpectColon: %d\tExpectValue: %d\tExpectComma: %d\n", index,token.type, ExpectKey, ExpectColon, ExpectValue, ExpectComma);
                      //  printf("line: %d\toffset: %d\n", token.line, token.offset);
                       // printf("token.type: %d\n", token.type);
                        //printf("last->type: %d\n", LastToken->type);
            throw std::runtime_error("[Parser::ParseObject]: expected comma");
        }
        else {
            ExpectComma = false;
            ExpectKey = true;
        }

        end:
        LastToken = &token;

    }
    --index;
    auto object = Object(obj);
    object.m_type = Object::Type::OBJECT;
    return object;
}



enum class State {
    START = 0,
    READ_ATTRIBUTE,
    READ_ARRAY,

    PARSER_DONE
};



Object Parser::Parse()
{
    Object obj;

    State state = State::START;
    bool bDone = false;
    unsigned index = 0;
    while(index < tokens.size())
    {
        Token& token = tokens[index];

        if(bDone){
            throw std::runtime_error("[Parser::Parse]: extra data found");
        }

        if(token.type == TokenType::BRACKET_OPEN){
            //++open;
            obj = ParseObject(index);
            bDone = true;
        }
        else if(token.type == TokenType::SQUARE_OPEN){
            //++open;
            obj = ParseArray(index);
            bDone = true;
        }
        else if(token.type == TokenType::NUMBER)
        {
            obj = Object(std::stold(token.value));
            bDone = true;
        }
        else if(token.type == TokenType::STRING)
        {
            obj = Object(token.value);
            bDone = true;
        }
        if(bDone) break;
        /*else if(token.type == TokenType::SQUARE_CLOSE){
            --open;
            if(open == 0) {state = State::PARSER_DONE; break;}
        }
        else if(token.type == TokenType::BRACKET_CLOSE){
            --open;
            if(open == 0){ state = State::PARSER_DONE; break;}
        }*/
 
        ++index;
    }
    //assert(state == State::PARSER_DONE);
    return obj;
}