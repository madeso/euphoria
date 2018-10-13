#include "duk/bindobject.h"

namespace duk
{
  ObjectBinder&
  ObjectBinder::AddFunction(const std::string& name, const std::shared_ptr<Overload>& bind)
  {
    functions.emplace_back(std::make_pair(name, bind));
    return *this;
  }

  ObjectBinder
  BindObject()
  {
    return ObjectBinder{};
  }
}