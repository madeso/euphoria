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
    std::vector<std::shared_ptr<Overload>> overloads;
  };

  template <typename... TArgs, typename Func>
  Bind
  MakeBind(Func callback)
  {
    Bind bind;
    bind.overloads.emplace_back(
        std::make_shared<GenericOverload<Func, TArgs...>>(callback));
    return bind;
  }
}

#endif  // EUPHORIA_DUK_BIND_H
