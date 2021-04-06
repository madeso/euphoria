#ifndef EUPHORIA_TYPEINFO_H
#define EUPHORIA_TYPEINFO_H

// typeinfo.h:
// Exposes macros and types for basic type information of known-at-compile-time
// type information. The type can be a simple as a int and as complex
// as a template argument
//
// Macros:
//
// TYPEID_ID(TYPE)
// The "integer" id of a type
//
// TYPEID_NAME(TYPE)
// The name of a type
//
// TYPEID_SETUP_TYPE(TYPE)
// Sets up typeinfo for a type
//
// Types:
//
// TypeId
// The "integer" type
//
// TypeName
// The "string" type
//
//
// How these are actually implemented is determined during
// the cmake configuration, but the following options are available:
//  * rtti: c++ rtti
//  * ctti: external library ctti
//  * custom rt: uses macros and runtime to calculate the id
//  * custom hashed: uses macros and fnv1a to calculate the id

#include "euph_generated_config.h"

#if BUILD_TYPEINFO_IMPL_CUSTOM_RT == 1 || BUILD_TYPEINFO_IMPL_CUSTOM_HASHED == 1
#else
namespace euphoria::core
{
    template <typename T>
    void
    TYPEINFO_UselessFunction();
}
#define TYPEID_SETUP_TYPE(X)                                                   \
    template <>                                                                \
    void euphoria::core::TYPEINFO_UselessFunction<X>()
#endif


#if BUILD_TYPEINFO_IMPL_RTTI == 1
// standard c++ library
// runtime only so it needs RTTI to function

#include <typeinfo>
#include <string>

namespace euphoria::core
{
    using TypeId   = std::size_t;
    using TypeName = std::string;
}  // namespace euphoria::core

#define TYPEID_ID(X) typeid(X).hash_code()
#define TYPEID_NAME(X) typeid(X).name()
#elif BUILD_TYPEINFO_IMPL_CTTI == 1
// external library, works compile-time
// doesn't seem to compile in windows vs2017
#include "ctti/type_id.hpp"
#include "ctti/nameof.hpp"

namespace euphoria::core
{
    using TypeId   = ctti::type_id_t;
    using TypeName = ctti::detail::cstring;

#define TYPEID_ID(X) ctti::type_id<X>()
#define TYPEID_NAME(X) ctti::nameof<X>()
}  // namespace euphoria::core

#elif BUILD_TYPEINFO_IMPL_CUSTOM_HASHED == 1                                   \
        || BUILD_TYPEINFO_IMPL_CUSTOM_RT == 1
// todo(Gustav): please implement this

#include <string>
#include <cstdint>

#if BUILD_TYPEINFO_IMPL_CUSTOM_HASHED == 1
#include "core/fnv1a.h"
#endif

namespace euphoria::core
{
    using TypeName = std::string;
    using TypeId   = std::uint64_t;

    template <typename T>
    struct TypeNameImpl
    {
        enum
        {
            HasValue = 0
        };
        static constexpr const char* const Name = "";
    };

#define TYPEID_SETUP_TYPE(X)                                                   \
    template <>                                                                \
    struct TypeNameImpl<X>                                                     \
    {                                                                          \
        enum                                                                   \
        {                                                                      \
            HasValue = 1                                                       \
        };                                                                     \
        static constexpr const char* const Name = #X;                          \
    }

    template <typename T>
    constexpr const char* const
    GetNameOf()
    {
        static_assert(
                TypeNameImpl<T>::HasValue == 1,
                "Type is missing implementation!");
        return TypeNameImpl<T>::Name;
    }

#define TYPEID_NAME(X) GetNameOf<X>()
}  // namespace euphoria::core

#if BUILD_TYPEINFO_IMPL_CUSTOM_RT == 1
namespace euphoria::core
{
    TypeId
    GetUniqueIdOf(const std::string& name);

#define TYPEID_ID(X) GetUniqueIdOf(TYPEID_NAME(X))
#else
#define TYPEID_ID(X) fnv1a(TYPEID_NAME(X))
#endif
}

#else
#error "Unhandled typeinfo implementation"
#endif

#endif  // EUPHORIA_TYPEINFO_H
