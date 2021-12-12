#pragma once

#define SOL_CHECK_ARGUMENTS 1

#include <string>
#include <limits> // seems like sol uses numeric_limits without including it...!

#include "sol/sol.hpp"

struct LuaState
{
    sol::state state;
    bool has_error = false;
    std::string error;
};
