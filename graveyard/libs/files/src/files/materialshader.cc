#include "files/materialshader.h"

#include "io/json.h"


namespace eu::files::materialshader
{
    JSON_PARSE_FUNC(Binding)
    {
        JSON_BEGIN_OBJECT();
        JSON_VAL(String, texture);
        JSON_VAL(String, uniform);
        JSON_END_OBJECT();
    }


    JSON_PARSE_FUNC(DefaultTexture)
    {
        JSON_BEGIN_OBJECT();
        JSON_VAL(String, texture);
        JSON_VAL(String, path);
        JSON_END_OBJECT();
    }


    JSON_PARSE_FUNC(MaterialShader)
    {
        JSON_BEGIN_OBJECT();

        JSON_ARRAY(Binding, textures);
        JSON_VAL(Bool, has_light);

        JSON_OPT_VAL(String, ambient);
        JSON_OPT_VAL(String, diffuse);
        JSON_OPT_VAL(String, specular);
        JSON_OPT_VAL(String, shininess);

        JSON_ARRAY(DefaultTexture, default_textures);

        JSON_END_OBJECT();
    }
}
