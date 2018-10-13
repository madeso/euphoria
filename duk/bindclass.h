#ifndef EUPHORIA_DUK_BINDCLASS_H
#define EUPHORIA_DUK_BINDCLASS_H

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <typeinfo>
#include <memory>

#include "duk/overload.h"

namespace duk
{
  class ClassBinder
  {
   public:
    explicit ClassBinder(size_t i);

    // todo: add constructor

    ClassBinder&
    SetConstructor(const std::shared_ptr<Overload>& bind);

    ClassBinder&
    AddMethod(const std::string& name, const std::shared_ptr<Overload>& bind);

    ClassBinder&
    AddProperty(
        const std::string&               name,
        const std::shared_ptr<Overload>& get,
        const std::shared_ptr<Overload>& set);

    size_t                    id;
    std::shared_ptr<Overload> constructor;
    std::vector<std::pair<std::string, std::shared_ptr<Overload>>> overloads;
    std::vector<std::tuple<
        std::string,
        std::shared_ptr<Overload>,
        std::shared_ptr<Overload>>>
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
