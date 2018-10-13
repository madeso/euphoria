#ifndef EUPHORIA_DUK_TEMPLATE_H
#define EUPHORIA_DUK_TEMPLATE_H

#include <vector>
#include <type_traits>
#include <typeinfo>

#include "core/cpp.h"
#include "core/assert.h"
#include "core/str.h"

#include "duk/config.h"
#include "duk/context.h"
#include "duk/prototype.h"
#include "duk/optional.h"

namespace duk
{
  std::string
  ArgumentError(int arg, const std::string& err);

  template <typename TT, typename TValid = void>
  struct DukTemplate
  {
  };

  template <typename TT>
  struct DukTemplate<
      TT,
      std::enable_if_t<
          !std::is_arithmetic<TT>::value && !IsVector<TT>::value &&
          !IsOptional<TT>::value>>
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
        const auto*     id      = ctx->GetObjectType(index);
        constexpr auto& cpptype = typeid(T);
        const auto*     self_type =
            ctx->TypeToProto(cpptype.hash_code() CLASS_ARG(cpptype.name()));
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
                Str() << "expected " << self_type->name << " but got "
                      << id->name);
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
      constexpr auto& cpptype = typeid(T);
      ASSERT(
          ctx->GetObjectType(index) ==
          ctx->TypeToProto(cpptype.hash_code() CLASS_ARG(cpptype.name())));
      return *static_cast<T*>(ctx->GetObjectPtr(index));
    }

    static std::string
    Name(Context* ctx)
    {
      constexpr auto& cpptype = typeid(T);
      auto*           proto =
          ctx->TypeToProto(cpptype.hash_code() CLASS_ARG(cpptype.name()));
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
  struct DukTemplate<T, std::enable_if_t<std::is_arithmetic<T>::value>>
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
  struct DukTemplate<std::string>
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
  struct DukTemplate<DukValue>
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

    static DukValue
    Parse(Context* ctx, int index)
    {
      ASSERT(ctx->IsObject(index));
      return DukValue{ctx->GetFunctionPtr(index)};
    }

    static std::string
    Name(Context*)
    {
      return "object";
    }
  };

  template <typename T>
  struct DukTemplate<std::vector<T>, void>
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
          const auto& match = DukTemplate<T>::CanMatch(ctx, -1, -1);
          ctx->StopArrayIndex();
          if(!match.empty())
          {
            return ArgumentError(
                arg,
                Str() << "array[" << i << "] (size: " << array_size
                      << ") has type error: "
                      << match);
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
        arr.emplace_back(DukTemplate<T>::Parse(ctx, -1));
        ctx->StopArrayIndex();
      }

      return arr;
    }

    static std::string
    Name(Context* ctx)
    {
      return Str() << "[" << DukTemplate<T>::Name(ctx) << "]";
    }
  };

  template <typename T>
  struct DukTemplate<Optional<T>, void>
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
        return DukTemplate<T>::CanMatch(ctx, index, arg);
      }
    }

    static Optional<T>
    Parse(Context* ctx, int index)
    {
      if(index >= 0)
      {
        return Optional<T>{};
      }
      else
      {
        return Optional<T>{DukTemplate<T>::Parse(ctx, index)};
      }
    }

    static std::string
    Name(Context* ctx)
    {
      return Str() << DukTemplate<T>::Name(ctx) << "?";
    }
  };
}

#endif  // EUPHORIA_DUK_TEMPLATE_H
