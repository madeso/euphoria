#ifndef EUPHORIA_TYPEINFO_H
#define EUPHORIA_TYPEINFO_H

#include "ctti/type_id.hpp"
#include "ctti/nameof.hpp"

using TypeId   = ctti::type_id_t;
using TypeName = ctti::detail::cstring;

#define TYPEID_ID(X) ctti::type_id<X>()
#define TYPEID_NAME(X) ctti::nameof<X>()


#endif  // EUPHORIA_TYPEINFO_H
