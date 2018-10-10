#include "core/duk_bindclass.h"

ClassBinder::ClassBinder(size_t i)
    : id(i)
{
}

ClassBinder&
ClassBinder::SetConstructor(const Bind& bind)
{
  constructor = bind;
  return *this;
}

ClassBinder&
ClassBinder::AddMethod(const std::string& name, const Bind& bind)
{
  overloads.emplace_back(std::make_pair(name, bind));
  return *this;
}

ClassBinder&
ClassBinder::AddProperty(
    const std::string& name, const Bind& get, const Bind& set)
{
  properties.emplace_back(std::make_tuple(name, get, set));
  return *this;
}
