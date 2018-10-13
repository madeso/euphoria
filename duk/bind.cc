#include "duk/bind.h"

namespace duk
{
  std::shared_ptr<Overload>
  MakeNoBind()
  {
    return std::shared_ptr<Overload>{};
  }
}
