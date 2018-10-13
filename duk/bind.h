#ifndef EUPHORIA_DUK_BIND_H
#define EUPHORIA_DUK_BIND_H

#include <memory>
#include <vector>

#include "duk/overload.h"
#include "duk/genericoverload.h"

namespace duk
{
  template <typename... TArgs, typename Func>
  std::shared_ptr<Overload>
  MakeBind(Func callback)
  {
    return std::make_shared<GenericOverload<Func, TArgs...>>(callback);
  }

  std::shared_ptr<Overload>
  MakeNoBind();
}

#endif  // EUPHORIA_DUK_BIND_H
