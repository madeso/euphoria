#include "duk/bindobject.h"


ObjectBinder&
ObjectBinder::AddFunction(const std::string& name, const Bind& bind)
{
  functions.emplace_back(std::make_pair(name, bind));
  return *this;
}

ObjectBinder
BindObject()
{
  return ObjectBinder{};
}
