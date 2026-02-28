#include "base/size.h"


namespace eu
{
    std::string string_from(const Size& v)
        { return fmt::format("{}x{}", v.width, v.height); }
}
