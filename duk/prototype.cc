#include "duk/prototype.h"

namespace duk
{
  Prototype::Prototype(const std::string& n, void* p)
      : name(n)
      , prototype(p)
  {
  }
}