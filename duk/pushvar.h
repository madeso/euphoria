#ifndef EUPHORIA_DUK_PUSHVARIMPL_H
#define EUPHORIA_DUK_PUSHVARIMPL_H

#include <string>
#include <memory>
#include <type_traits>

#include "duk/context.h"
#include "core/cpp.h"

namespace euphoria::duk
{
    template <typename T, typename TValid = void>
    struct PushVarImpl
    {};

    template <typename T>
    struct PushVarImpl<T, std::enable_if_t<std::is_class<T>::value>>
    {
        static void
        Push(Context* ctx, const T& arg)
        {
            ctx->ReturnObject<T>(std::make_shared<T>(arg));
        }
    };

    template <typename T>
    struct PushVarImpl<T, std::enable_if_t<std::is_arithmetic<T>::value>>
    {
        static void
        Push(Context* ctx, const T& arg)
        {
            ctx->ReturnNumber(arg);
        }
    };

    template <>
    struct PushVarImpl<float>
    {
        static void
        Push(Context* ctx, const float& arg)
        {
            ctx->ReturnNumber(arg);
        }
    };

    template <>
    struct PushVarImpl<std::string>
    {
        static void
        Push(Context* ctx, const std::string& arg)
        {
            ctx->ReturnString(arg);
        }
    };

    template <>
    struct PushVarImpl<const char*>
    {
        static void
        Push(Context* ctx, const std::string& arg)
        {
            ctx->ReturnString(arg);
        }
    };

    template <typename T>
    void
    PushVar(Context* ctx, const T& arg)
    {
        // *Impl struct here to support template-ing char* strings
        // https://stackoverflow.com/a/6559891/180307
        typedef typename core::ArrayToPointerDecay<T>::Type Type;
        PushVarImpl<Type>::Push(ctx, arg);
    }
}  // namespace euphoria::duk

#endif  // EUPHORIA_DUK_PUSHVARIMPL_H
