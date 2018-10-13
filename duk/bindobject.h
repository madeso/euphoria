#ifndef EUPHORIA_DUK_BINDOBJECT_H
#define EUPHORIA_DUK_BINDOBJECT_H

#include <string>
#include <vector>

#include "duk/bind.h"

namespace duk
{
  class ObjectBinder
  {
   public:
    ObjectBinder&
    AddFunction(const std::string& name, const std::shared_ptr<Overload>& bind);

    std::vector<std::pair<std::string, std::shared_ptr<Overload>>> functions;
  };

  ObjectBinder
  BindObject();
}

#endif  // EUPHORIA_DUK_BINDOBJECT_H
