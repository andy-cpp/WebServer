#pragma once

#include "Object.hpp"

namespace JSON
{
    Object Parse(std::string const& source);

    Object LoadFromFile(std::string const& filename);

    std::string ToString(Object const& object);
}