#ifndef EUPHORIA_DUK_OVERLOAD_H
#define EUPHORIA_DUK_OVERLOAD_H

#include <string>

class Context;

class Overload
{
 public:
  Overload()          = default;
  virtual ~Overload() = default;

  // return empty string if matches
  // non-empty means match failed
  virtual std::string
  Matches(Context* ctx) = 0;

  virtual int
  Call(Context* ctx) = 0;

  virtual std::string
  Describe(Context* context) const = 0;
};


#endif  // EUPHORIA_DUK_OVERLOAD_H
