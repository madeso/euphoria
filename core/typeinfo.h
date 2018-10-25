#ifndef EUPHORIA_TYPEINFO_H
#define EUPHORIA_TYPEINFO_H

#include "euph_generated_config.h"

// typeinfo.h:
//
// basically a wrapper over the ctti library
// why? because if ctti fails to compile or proves to be too hard
// we can easily switch back to typeof() and std::typeinfo,
// another library
// or a custom solution

template <typename T>
void
TYPEINFO_UselessFunction();
#define TYPEID_SETUP_TYPE(X) \
  template <>                \
  void TYPEINFO_UselessFunction<X>()

#if BUILD_TYPEINFO_IMPL_RTTI == 1
// standard c++ library
// runtime only so it needs RTTI to function

#include <typeinfo>
#include <string>

using TypeId   = std::size_t;
using TypeName = std::string;

#define TYPEID_ID(X) typeid(X).hash_code()
#define TYPEID_NAME(X) typeid(X).name()
#elif BUILD_TYPEINFO_IMPL_CTTI == 1
// external library, works compile-time
// doesn't seem to compile in windows vs2017
#include "ctti/type_id.hpp"
#include "ctti/nameof.hpp"

using TypeId   = ctti::type_id_t;
using TypeName = ctti::detail::cstring;

#define TYPEID_ID(X) ctti::type_id<X>()
#define TYPEID_NAME(X) ctti::nameof<X>()

#elif BUILD_TYPEINFO_IMPL_CUSTOM_HASHED == 1 || \
    BUILD_TYPEINFO_IMPL_CUSTOM_RT == 1
// todo: please implement this

#include <string>

using TypeName = std::string;
using TypeId   = unsigned int;

template <typename T>
struct TypeNameImpl
{
  enum
  {
    HasValue = 0
  };
  static constexpr const char* const Name = "";
};

#undef TYPEID_SETUP_TYPE
#define TYPEID_SETUP_TYPE(X)                      \
  template <>                                     \
  struct TypeNameImpl<X>                          \
  {                                               \
    enum                                          \
    {                                             \
      HasValue = 1                                \
    };                                            \
    static constexpr const char* const Name = #X; \
  }

template <typename T>
constexpr const char* const
GetNameOf()
{
  static_assert(
      TypeNameImpl<T>::HasValue == 1, "Type is missing implementation!");
  return TypeNameImpl<T>::Name;
}

#define TYPEID_NAME(X) GetNameOf<X>()

#if BUILD_TYPEINFO_IMPL_CUSTOM_RT == 1

TypeId
GetUniqueIdOf(const std::string& name);

#define TYPEID_ID(X) GetUniqueIdOf(TYPEID_NAME(X))
#else
#error "todo: not implemented"
#endif


#else
#error "Unhandled typeinfo implementation"
#endif


#endif  // EUPHORIA_TYPEINFO_H
