#ifndef EUPHORIA_DUK_BINDFUNCTION_H
#define EUPHORIA_DUK_BINDFUNCTION_H

#include <memory>
#include <vector>

class Overload;

class Function
{
 public:
  Function() = default;
  virtual ~Function();

  std::vector<std::shared_ptr<Overload>> overloads;
};

#endif  // EUPHORIA_DUK_BINDFUNCTION_H
