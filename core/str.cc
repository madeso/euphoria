#include "str.h"
#include <cassert>
#include <string>

Str::operator std::string() const
{
  return ToString();
}

std::string
Str::ToString() const
{
  return stream_.str();
}
