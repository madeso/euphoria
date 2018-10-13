#include "duk/bind.h"

namespace duk
{
  std::shared_ptr<Function>
  MakeNoBind()
  {
    return std::shared_ptr<Function>{};
  }
}
