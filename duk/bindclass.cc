#include "duk/bindclass.h"

namespace duk
{
  ClassBinder::ClassBinder(size_t i)
      : id(i)
  {
  }

  ClassBinder&
  ClassBinder::SetConstructor(const std::shared_ptr<Function>& bind)
  {
    constructor = bind;
    return *this;
  }

  ClassBinder&
  ClassBinder::AddMethod(
      const std::string& name, const std::shared_ptr<Function>& bind)
  {
    overloads.emplace_back(std::make_pair(name, bind));
    return *this;
  }

  ClassBinder&
  ClassBinder::AddProperty(
      const std::string&               name,
      const std::shared_ptr<Function>& get,
      const std::shared_ptr<Function>& set)
  {
    properties.emplace_back(std::make_tuple(name, get, set));
    return *this;
  }
}
