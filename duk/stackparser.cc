#include "duk/stackparser.h"

namespace euphoria::duk
{
  std::string
  ArgumentError(int arg, const std::string& err)
  {
    if(arg < 0)
    {
      return err;
    }
    std::string th;
    switch(arg % 10)
    {
      case 1:
        th = "st";
        break;
      case 2:
        th = "nd";
        break;
      case 3:
        th = "rd";
        break;
      default:
        th = "th";
        break;
    }
    return core::Str() << arg << th << " arg, " << err;
  }
}