#include "duk/bind.h"

namespace euphoria::duk
{
    std::shared_ptr<Function>
    MakeNoBind()
    {
        return std::shared_ptr<Function> {};
    }
}  // namespace euphoria::duk
