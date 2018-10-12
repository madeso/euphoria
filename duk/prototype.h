#ifndef EUPHORIA_DUK_PROTOTYPE_H
#define EUPHORIA_DUK_PROTOTYPE_H

#include <string>

namespace duk
{
  class Prototype
  {
   public:
    Prototype(const std::string& n, void* p);

    std::string name;
    void*       prototype;
  };
}

#endif  // EUPHORIA_DUK_PROTOTYPE_H
