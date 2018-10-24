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

#include "ctti/type_id.hpp"
#include "ctti/nameof.hpp"

using TypeId   = ctti::type_id_t;
using TypeName = ctti::detail::cstring;

#define TYPEID_ID(X) ctti::type_id<X>()
#define TYPEID_NAME(X) ctti::nameof<X>()

#elif BUILD_TYPEINFO_IMPL_CUSTOM == 1
// todo: please implement this
#else
#error "Unhandled typeinfo implementation"
#endif


#endif  // EUPHORIA_TYPEINFO_H
