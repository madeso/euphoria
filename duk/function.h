#ifndef EUPHORIA_DUK_BINDFUNCTION_H
#define EUPHORIA_DUK_BINDFUNCTION_H

#include <string>

namespace duk
{
  class Context;

  class Function
  {
   public:
    Function() = default;

    virtual ~Function() = default;

    // return empty string if matches
    // non-empty means match failed
    virtual std::string
    Matches(Context* ctx) = 0;

    virtual int
    Call(Context* ctx) = 0;

    virtual std::string
    Describe(Context* context) const = 0;
  };
}

#endif  // EUPHORIA_DUK_BINDFUNCTION_H
