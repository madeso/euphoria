#ifndef EUPHORIA_DUK_BINDOBJECT_H
#define EUPHORIA_DUK_BINDOBJECT_H

#include <string>
#include <vector>

#include "duk/bind.h"

namespace euphoria::duk
{
  class ObjectBinder
  {
   public:
    ObjectBinder&
    AddFunction(const std::string& name, const std::shared_ptr<Function>& bind);

    struct MemberFunction
    {
      std::string               name;
      std::shared_ptr<Function> function;
    };

    std::vector<MemberFunction> functions;
  };

  ObjectBinder
  BindObject();
}

#endif  // EUPHORIA_DUK_BINDOBJECT_H
