#include "files/world.h"

namespace eu::files::world
{
    JSON_PARSE_FUNC(vec2f)
    {
        JSON_BEGIN_OBJECT();
        
        JSON_VAL(Number, x);
        JSON_VAL(Number, y);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(Property)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, name);
        JSON_VAL_OPT_OBJ(vec2f, vec2f_value);
        
        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(Object)
    {
        JSON_BEGIN_OBJECT();
        
        JSON_VAL(String, template_name);
        JSON_ARRAY(Property, properties);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(World)
    {
        JSON_BEGIN_OBJECT();

        JSON_ARRAY(Object, objects);

        JSON_END_OBJECT();
    }
}
