#include "files/mesh.h"

#include "io/json.h"


namespace eu::files::mesh
{
    JSON_PARSE_FUNC(Texture)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, type);
        JSON_VAL(String, path);

        JSON_END_OBJECT();
    }
    

    JSON_PARSE_FUNC(Material)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, name);
        JSON_ARRAY(Texture, textures);

        JSON_END_OBJECT();
    }
    

    JSON_PARSE_FUNC(Mesh)
    {
        JSON_BEGIN_OBJECT();

        JSON_ARRAY(Material, materials);
        JSON_OPT_VAL(Bool, diffuse_and_ambient_are_same);

        JSON_END_OBJECT();
    }
    

    JSON_PARSE_FUNC(Extension)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, old_ext);
        JSON_VAL(String, new_ext);

        JSON_END_OBJECT();
    }


    JSON_PARSE_FUNC(Filename)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, old_file);
        JSON_VAL(String, new_file);

        JSON_END_OBJECT();
    }
    

    JSON_PARSE_FUNC(Folder)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, texture_override);
        JSON_ARRAY(Extension, change_extensions);
        JSON_ARRAY(Filename, change_filenames);
        JSON_OPT_VAL(Number, scale);

        JSON_END_OBJECT();
    }
}
