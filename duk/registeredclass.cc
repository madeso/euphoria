#include "duk/registeredclass.h"

namespace duk
{
  RegisteredClass::RegisteredClass(const std::string& n, void* p)
      : name(n)
      , prototype(p)
  {
  }
}