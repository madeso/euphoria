#include "duk/registeredclass.h"

namespace euphoria::duk
{
    RegisteredClass::RegisteredClass(const std::string& n, void* p)
        : name(n), prototype(p)
    {}
}  // namespace euphoria::duk