#ifndef EUPHORIA_DUK_BIND_H
#define EUPHORIA_DUK_BIND_H

#include <memory>
#include <vector>

#include "duk/overload.h"
#include "duk/genericoverload.h"

namespace duk
{
  class Bind
  {
   public:
    template <typename... TArgs, typename Func>
    Bind&
    bind(Func callback)
    {
      overloads.emplace_back(
          std::make_shared<GenericOverload<Func, TArgs...>>(callback));
      return *this;
    }

    std::vector<std::shared_ptr<Overload>> overloads;
  };
}

#endif  // EUPHORIA_DUK_BIND_H
