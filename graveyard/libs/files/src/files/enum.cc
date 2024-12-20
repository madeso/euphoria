#include "files/enum.h"

namespace eu::files::enums
{
    JSON_PARSE_FUNC(Root)
    {
        JSON_BEGIN_OBJECT();

        JSON_ARRAY_PROP(String, names);

        JSON_END_OBJECT();
    }
}
