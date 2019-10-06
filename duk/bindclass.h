#ifndef EUPHORIA_DUK_BINDCLASS_H
#define EUPHORIA_DUK_BINDCLASS_H

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <memory>

#include "core/typeinfo.h"

#include "duk/function.h"

namespace euphoria::duk
{
    class ClassBinder
    {
        public:
        explicit ClassBinder(core::TypeId i);

        // todo: add constructor

        ClassBinder&
        SetConstructor(const std::shared_ptr<Function>& bind);

        ClassBinder&
        AddMethod(
                const std::string&               name,
                const std::shared_ptr<Function>& bind);

        ClassBinder&
        AddProperty(
                const std::string&               name,
                const std::shared_ptr<Function>& get,
                const std::shared_ptr<Function>& set);

        core::TypeId              id;
        std::shared_ptr<Function> constructor;

        struct Method
        {
            std::string               name;
            std::shared_ptr<Function> function;
        };

        struct Property
        {
            std::string               name;
            std::shared_ptr<Function> get;
            std::shared_ptr<Function> set;
        };

        std::vector<Method>   methods;
        std::vector<Property> properties;
    };

    template <typename T>
    ClassBinder
    BindClass()
    {
        return ClassBinder {TYPEID_ID(T)};
    }
}  // namespace euphoria::duk

#endif  // EUPHORIA_DUK_BINDCLASS_H
