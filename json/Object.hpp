#pragma once
#include <string>
#include <assert.h>
#include <map>
#include <vector>
/*
    JSON Object wrapper
*/

typedef void(*destructor_t)(void* object);
template <typename T>
destructor_t CreateDestructor()
{
    return [](void* object){
        delete (T*)object;
    };
}

typedef void(*copy_t)(void*& buffer, void* object);
template <typename T>
copy_t CreateCopy()
{
    return [](void*& buffer, void* object)
    {
        (T*&)buffer = new T(*(T*)object);
    };
}
class Object;


class Object
{


public:
    typedef std::map<std::string, Object> dictionary_t;
    typedef std::vector<Object> array_t;

    enum class Type {
        ERROR = 0,
        OBJECT,
        ARRAY,
        STRING,
        NULL_,
        BOOL,
        NUMBER
    };

    template <typename T>
    void Init()
    {
        m_copy = CreateCopy<T>();
        m_destructor = CreateDestructor<T>();
    }

    Object() = default;

    // Array initializer
    Object(std::initializer_list<Object> list)
    {
        Init<array_t>();
        m_type = Type::ARRAY;
        m_buffer = new array_t(list);
    }

    Object(std::initializer_list<std::pair<std::string, Object>> list)
    {
        Init<dictionary_t>();
        m_type = Type::OBJECT;
        m_buffer = new dictionary_t;
        dictionary_t& obj = *(dictionary_t*)m_buffer;
        for(auto& entry : list)
        {
            obj[entry.first] = entry.second;
        }
    }

    Object(char const* str)
    {
        Init<std::string>();
        m_type = Type::STRING;
        m_buffer = new std::string(str);
    }

    Object(std::string const& str)
    {
        Init<std::string>();
        m_type = Type::STRING;
        m_buffer = new std::string(str);
    }

    Object(long double integer)
    {
        Init<long double>();
        m_type = Type::NUMBER;
        m_buffer = new long double(integer);
    }

    Object(array_t const& array)
    {
        Init<array_t>();
        m_type = Type::ARRAY;
        m_buffer = new array_t(array);
    }

    Object(dictionary_t const& obj)
    {
        Init<dictionary_t>();
        m_type = Type::OBJECT;
        m_buffer = new dictionary_t(obj);
    }

    template <bool>
    Object(bool const& value)
    {
        Init<bool>();
        m_type = Type::BOOL;
        m_buffer = new bool(value);
    }

    Object(Object const& other)
    {
        if(m_buffer)
            m_destructor(m_buffer);
        m_type = other.m_type;
        m_copy = other.m_copy;
        m_destructor = other.m_destructor;
        if(m_copy)
            m_copy(m_buffer, other.m_buffer);
    }

    Object& operator=(Object const& other)
    {
                if(m_buffer)
            m_destructor(m_buffer);
        m_type = other.m_type;
        m_copy = other.m_copy;
        m_destructor = other.m_destructor;
        if(m_copy)
            m_copy(m_buffer, other.m_buffer);
        return *this;
    }

    ~Object()
    {
        if(m_destructor)
            m_destructor(m_buffer);
        else free(m_buffer);
    }

    // Dumps object to string
    std::string Dump() const;

    // Sets value
    void Set(Object const& value);

    void Set(std::string const& key, Object const& value);

    // Get from dictionary
    Object& Get(std::string const& key);

    Object const& Get(std::string const& key) const;

    Object& operator[](std::string const& key)
    {
        assert(m_type == Type::OBJECT);
        dictionary_t* dict = (dictionary_t*)m_buffer;

        return dict->at(key);
    }

    Object& operator[](unsigned index)
    {
        assert(m_type == Type::ARRAY);
        return ((array_t*)m_buffer)->at(index);
    }

    std::string& ToString()
    {
        assert(m_type == Type::STRING);
        return *(std::string*)m_buffer;
    }

    std::string const& ToString() const
    {
        assert(m_type == Type::STRING);
        return *(std::string*)m_buffer;
    }
    

    dictionary_t& ToDictionary()
    {
        assert(m_type == Type::OBJECT);
        return *(dictionary_t*)m_buffer;
    }

    
    dictionary_t const& ToDictionary() const
    {
        assert(m_type == Type::OBJECT);
        return *(dictionary_t*)m_buffer;
    }

    array_t& ToArray()
    {
        assert(m_type == Type::ARRAY);
        return *(array_t*)m_buffer;
    }

    
    array_t const& ToArray() const
    {
        assert(m_type == Type::ARRAY);
        return *(array_t*)m_buffer;
    }

    bool IsNull() const
    {
        return m_type == Type::NULL_;
    }

    bool& ToBool()
    {
        assert(m_type == Type::BOOL);
        return *(bool*)m_buffer;
    }
    
    bool const& ToBool() const
    {
        assert(m_type == Type::BOOL);
        return *(bool*)m_buffer;
    }

    long double& ToNumber()
    {
        assert(m_type == Type::NUMBER);
        return *(long double*)m_buffer;
    }

        long double const& ToNumber() const
    {
        assert(m_type == Type::NUMBER);
        return *(long double*)m_buffer;
    }
private:
    

private:
    destructor_t m_destructor = 0;
    copy_t m_copy = 0;

public:
    Type m_type = Type::NULL_;
    void* m_buffer = 0;
    
};

