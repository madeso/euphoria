#ifndef EUPHORIA_DUK_BINDFUNCTION_H
#define EUPHORIA_DUK_BINDFUNCTION_H

#include <memory>
#include <vector>

namespace duk
{
  class Overload;

  class Function
  {
   public:
    Function() = default;

    ~Function();

    std::shared_ptr<Overload> overloads;
  };
}

#endif  // EUPHORIA_DUK_BINDFUNCTION_H
