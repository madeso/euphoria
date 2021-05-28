#pragma once

#define SOL_CHECK_ARGUMENTS 1

#include <string>

#include "sol/sol.hpp"

struct Sol
{
  sol::state lua;
  bool has_error = false;
  std::string error;
};
