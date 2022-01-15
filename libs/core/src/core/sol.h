#pragma once

#define SOL_CHECK_ARGUMENTS 1

#include <string>
#include <limits> // seems like sol uses numeric_limits without including it...!

#include "core/compiler_warning.h"


DISABLE_WARNING_PUSH
    DISABLE_GLOBAL_DECLARATION
    #include "sol/sol.hpp"
DISABLE_WARNING_POP



struct LuaState
{
    sol::state state;
    bool has_error = false;
    std::string error;
};
