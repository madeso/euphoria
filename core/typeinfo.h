#ifndef EUPHORIA_TYPEINFO_H
#define EUPHORIA_TYPEINFO_H

#include <string>

struct TypeInfo
{
  TypeInfo() = default;

  const char* const name = "";
  int               id   = 0;

  bool
  operator==(const TypeInfo& rhs) const
  {
    return true;
  }
};

#define TYPEID(X) \
  TypeInfo        \
  {               \
  }


#endif  // EUPHORIA_TYPEINFO_H
