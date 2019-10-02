#include "duk/bindobject.h"

namespace euphoria::duk
{
  ObjectBinder&
  ObjectBinder::AddFunction(
      const std::string& name, const std::shared_ptr<Function>& bind)
  {
    MemberFunction func;
    func.name     = name;
    func.function = bind;
    functions.emplace_back(func);
    return *this;
  }

  ObjectBinder
  BindObject()
  {
    return ObjectBinder{};
  }
}