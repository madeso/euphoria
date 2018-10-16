#ifndef EUPHORIA_TYPEINFO_H
#define EUPHORIA_TYPEINFO_H

// typeinfo.h:
//
// basically a wrapper over the ctti library
// why? because if ctti fails to compile or proves to be too hard
// we can easily switch back to typeof() and std::typeinfo,
// another library
// or a custom solution

#include "ctti/type_id.hpp"
#include "ctti/nameof.hpp"

using TypeId   = ctti::type_id_t;
using TypeName = ctti::detail::cstring;

#define TYPEID_ID(X) ctti::type_id<X>()
#define TYPEID_NAME(X) ctti::nameof<X>()


#endif  // EUPHORIA_TYPEINFO_H
