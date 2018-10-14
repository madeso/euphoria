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
    Method m;
    m.name     = name;
    m.function = bind;
    methods.emplace_back(m);
    return *this;
  }

  ClassBinder&
  ClassBinder::AddProperty(
      const std::string&               name,
      const std::shared_ptr<Function>& get,
      const std::shared_ptr<Function>& set)
  {
    Property prop;
    prop.name = name;
    prop.get  = get;
    prop.set  = set;
    properties.emplace_back(prop);
    return *this;
  }
}
