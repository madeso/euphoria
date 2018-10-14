#ifndef EUPHORIA_DUK_BINDCLASS_H
#define EUPHORIA_DUK_BINDCLASS_H

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <typeinfo>
#include <memory>

#include "duk/function.h"

namespace duk
{
  class ClassBinder
  {
   public:
    explicit ClassBinder(size_t i);

    // todo: add constructor

    ClassBinder&
    SetConstructor(const std::shared_ptr<Function>& bind);

    ClassBinder&
    AddMethod(const std::string& name, const std::shared_ptr<Function>& bind);

    ClassBinder&
    AddProperty(
        const std::string&               name,
        const std::shared_ptr<Function>& get,
        const std::shared_ptr<Function>& set);

    size_t                    id;
    std::shared_ptr<Function> constructor;

    struct Method
    {
      std::string               name;
      std::shared_ptr<Function> function;
    };

    std::vector<Method> overloads;
    std::vector<std::tuple<
        std::string,
        std::shared_ptr<Function>,
        std::shared_ptr<Function>>>
        properties;
  };

  template <typename T>
  ClassBinder
  BindClass()
  {
    return ClassBinder{typeid(T).hash_code()};
  }
}

#endif  // EUPHORIA_DUK_BINDCLASS_H
