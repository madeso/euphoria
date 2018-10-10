#ifndef EUPHORIA_DUK_BINDOBJECT_H
#define EUPHORIA_DUK_BINDOBJECT_H

#include <string>
#include <vector>

#include "core/duk_bind.h"

class ObjectBinder
{
 public:
  ObjectBinder&
  AddFunction(const std::string& name, const Bind& bind);

  std::vector<std::pair<std::string, Bind>> functions;
};

ObjectBinder
BindObject();

#endif  // EUPHORIA_DUK_BINDOBJECT_H
