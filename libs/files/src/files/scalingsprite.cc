#include "files/scalingsprite.h"

#include "io/json.h"


namespace eu::files::scalingsprite
{
    JSON_PARSE_FUNC(Rect)
    {
        JSON_BEGIN_OBJECT();
        JSON_VAL(Int, left);
        JSON_VAL(Int, right);
        JSON_VAL(Int, top);
        JSON_VAL(Int, bottom);
        JSON_END_OBJECT();
    }

    namespace
    {
        int c_tint_to_int(jsonh::tint t)
        {
            // todo(Gustav): add error handling for when tint is larger than int
            return static_cast<int>(t);
        }
    }

    JSON_PARSE_FUNC(ScalingSprite)
    {
        JSON_BEGIN_OBJECT();
        JSON_ARRAY_PROP_CON(Int, rows, c_tint_to_int);
        JSON_ARRAY_PROP_CON(Int, cols, c_tint_to_int);
        JSON_ARRAY(Rect, rects);
        JSON_END_OBJECT();
    }
}
