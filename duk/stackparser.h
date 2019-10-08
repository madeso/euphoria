#ifndef EUPHORIA_DUK_TEMPLATE_H
#define EUPHORIA_DUK_TEMPLATE_H

#include <vector>
#include <type_traits>

#include "core/typeinfo.h"
#include "core/cpp.h"
#include "core/assert.h"
#include "core/str.h"

#include "duk/config.h"
#include "duk/context.h"
#include "duk/registeredclass.h"
#include "duk/optional.h"

namespace euphoria::duk
{
    std::string
    ArgumentError(int arg, const std::string& err);

    template <typename TT, typename TValid = void>
    struct StackParser
    {};

    template <typename TT>
    struct StackParser<
            TT,
            std::enable_if_t<
                    !std::is_arithmetic<TT>::value && !core::IsVector<TT>::value
                    && !IsOptional<TT>::value>>
    {
        using T = typename std::decay<TT>::type;

        static bool
        IsRequired()
        {
            return true;
        }

        static std::string
        CanMatch(Context* ctx, int index, int arg)
        {
            if(ctx->IsObject(index))
            {
                const auto* id        = ctx->GetObjectType(index);
                const auto* self_type = ctx->TypeToProto(
                        TYPEID_ID(T) CLASS_ARG(TYPEID_NAME(T)));
                ASSERT(self_type != nullptr);
                if(id == self_type)
                {
                    return "";
                }
                else
                {
                    if(self_type)
                    {
                        return ArgumentError(
                                arg,
                                core::Str() << "expected " << self_type->name
                                            << " but got " << id->name);
                    }
                    else
                    {
                        DIE("failed to get type");
                        return ArgumentError(arg, "failed to get type");
                    }
                }
            }
            else
            {
                return ArgumentError(arg, "not a object");
            }
        }

        static T&
        Parse(Context* ctx, int index)
        {
            ASSERT(ctx->GetObjectType(index)
                   == ctx->TypeToProto(TYPEID_ID(T) CLASS_ARG(TYPEID_NAME(T))));
            return *static_cast<T*>(ctx->GetObjectPtr(index));
        }

        static std::string
        Name(Context* ctx)
        {
            auto* proto
                    = ctx->TypeToProto(TYPEID_ID(T) CLASS_ARG(TYPEID_NAME(T)));
            if(proto)
            {
                return proto->name;
            }
            else
            {
                DIE("failed to get prototype");
                return "???";
            }
        }
    };

    template <typename T>
    struct StackParser<T, std::enable_if_t<std::is_arithmetic<T>::value>>
    {
        static bool
        IsRequired()
        {
            return true;
        }

        static std::string
        CanMatch(Context* ctx, int index, int arg)
        {
            if(ctx->IsNumber(index))
            {
                // todo: validate if is within T range
                return "";
            }
            else
            {
                return ArgumentError(arg, "not a number");
            }
        }

        static T
        Parse(Context* ctx, int index)
        {
            return static_cast<T>(ctx->GetNumber(index));
        }

        static std::string
        Name(Context*)
        {
            return "number";
        }
    };

    template <>
    struct StackParser<std::string>
    {
        static bool
        IsRequired()
        {
            return true;
        }

        static std::string
        CanMatch(Context* ctx, int index, int arg)
        {
            if(ctx->IsString(index))
            {
                return "";
            }
            else
            {
                return ArgumentError(arg, "not a string");
            }
        }

        static std::string
        Parse(Context* ctx, int index)
        {
            return ctx->GetString(index);
        }

        static std::string
        Name(Context*)
        {
            return "string";
        }
    };

    template <>
    struct StackParser<ObjectReference>
    {
        static bool
        IsRequired()
        {
            return true;
        }

        static std::string
        CanMatch(Context* ctx, int index, int arg)
        {
            if(ctx->IsObject(index))
            {
                return "";
            }
            else
            {
                return ArgumentError(arg, "not a object");
            }
        }

        static ObjectReference
        Parse(Context* ctx, int index)
        {
            ASSERT(ctx->IsObject(index));
            return ObjectReference {ctx->GetFunctionPtr(index)};
        }

        static std::string
        Name(Context*)
        {
            return "object";
        }
    };

    template <typename T>
    struct StackParser<std::vector<T>, void>
    {
        static bool
        IsRequired()
        {
            return true;
        }

        static std::string
        CanMatch(Context* ctx, int index, int arg)
        {
            if(ctx->IsArray(index))
            {
                const auto array_size = ctx->GetArrayLength(index);
                for(int i = 0; i < array_size; i += 1)
                {
                    ctx->GetArrayIndex(index, i);
                    const auto& match = StackParser<T>::CanMatch(ctx, -1, -1);
                    ctx->StopArrayIndex();
                    if(!match.empty())
                    {
                        return ArgumentError(
                                arg,
                                core::Str() << "array[" << i
                                            << "] (size: " << array_size
                                            << ") has type error: " << match);
                    }
                }

                return "";
            }
            else
            {
                return ArgumentError(arg, "not a array");
            }
        }

        static std::vector<T>
        Parse(Context* ctx, int index)
        {
            std::vector<T> arr;

            const auto array_size = ctx->GetArrayLength(index);
            for(int i = 0; i < array_size; i += 1)
            {
                ctx->GetArrayIndex(index, i);
                arr.emplace_back(StackParser<T>::Parse(ctx, -1));
                ctx->StopArrayIndex();
            }

            return arr;
        }

        static std::string
        Name(Context* ctx)
        {
            return core::Str() << "[" << StackParser<T>::Name(ctx) << "]";
        }
    };

    template <typename T>
    struct StackParser<Optional<T>, void>
    {
        static bool
        IsRequired()
        {
            return false;
        }

        static std::string
        CanMatch(Context* ctx, int index, int arg)
        {
            if(index >= 0)
            {
                return "";
            }
            else
            {
                return StackParser<T>::CanMatch(ctx, index, arg);
            }
        }

        static Optional<T>
        Parse(Context* ctx, int index)
        {
            if(index >= 0)
            {
                return Optional<T> {};
            }
            else
            {
                return Optional<T> {StackParser<T>::Parse(ctx, index)};
            }
        }

        static std::string
        Name(Context* ctx)
        {
            return core::Str() << StackParser<T>::Name(ctx) << "?";
        }
    };
}  // namespace euphoria::duk

#endif  // EUPHORIA_DUK_TEMPLATE_H
