#ifndef EUPHORIA_DUK_BIND_H
#define EUPHORIA_DUK_BIND_H

#include <memory>
#include <vector>

#include "duk/function.h"
#include "duk/genericfunction.h"

namespace euphoria::duk
{
    template <typename... TArgs, typename Func>
    std::shared_ptr<Function>
    MakeBind(Func callback)
    {
        return std::make_shared<GenericFunction<Func, TArgs...>>(callback);
    }

    std::shared_ptr<Function>
    MakeNoBind();
}  // namespace euphoria::duk

#endif  // EUPHORIA_DUK_BIND_H
