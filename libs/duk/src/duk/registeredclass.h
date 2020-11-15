#ifndef EUPHORIA_DUK_PROTOTYPE_H
#define EUPHORIA_DUK_PROTOTYPE_H

#include <string>

namespace euphoria::duk
{
    struct RegisteredClass
    {
    public:
        RegisteredClass(const std::string& n, void* p);

        std::string name;
        void*       prototype;
    };
}  // namespace euphoria::duk

#endif  // EUPHORIA_DUK_PROTOTYPE_H
