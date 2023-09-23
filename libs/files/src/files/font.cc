#include "files/font.h"

#include "io/json.h"


namespace eu::files::font
{
    JSON_PARSE_FUNC(FontFile)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, file);
        JSON_VAL(String, characters);

        JSON_END_OBJECT();
    }


    JSON_PARSE_FUNC(SingleImage)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, file);
        JSON_VAL(String, alias);
        JSON_OPT_VAL(Number, scale );
        JSON_VAL(Int, bearing_x);
        JSON_VAL(Int, bearing_y);
        JSON_VAL(Int, advance);

        JSON_END_OBJECT();
    }


    JSON_PARSE_FUNC(Root)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(Int, size);
        JSON_ARRAY(FontFile, fonts);
        JSON_ARRAY(SingleImage, images);
        JSON_OPT_VAL(Bool, builtin8);
        JSON_OPT_VAL(Bool, builtin13);

        JSON_END_OBJECT();
    }
}
