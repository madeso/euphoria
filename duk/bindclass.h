#ifndef EUPHORIA_DUK_BINDCLASS_H
#define EUPHORIA_DUK_BINDCLASS_H

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <typeinfo>

#include "duk/bind.h"

namespace duk
{
  class ClassBinder
  {
   public:
    explicit ClassBinder(size_t i);

    // todo: add constructor

    ClassBinder&
    SetConstructor(const Bind& bind);

    ClassBinder&
    AddMethod(const std::string& name, const Bind& bind);

    ClassBinder&
    AddProperty(const std::string& name, const Bind& get, const Bind& set);

    size_t id;
    Bind   constructor;
    std::vector<std::pair<std::string, Bind>> overloads;
    std::vector<std::tuple<std::string, Bind, Bind>> properties;
  };

  template <typename T>
  ClassBinder
  BindClass()
  {
    return ClassBinder{typeid(T).hash_code()};
  }
}

#endif  // EUPHORIA_DUK_BINDCLASS_H
