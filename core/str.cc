#include "str.h"
#include <cassert>
#include <string>

Str::operator std::string() const {
  assert(this);
  return ToString();
}

std::string Str::ToString() const {
  assert(this);
  return stream_.str();
}

