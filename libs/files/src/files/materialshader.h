#pragma once

#include "io/json.h"


namespace eu::files::materialshader
{
    struct Binding
    {
        std::string texture;
        std::string uniform;
    };

    struct DefaultTexture
    {
        std::string texture;
        std::string path;
    };

    struct MaterialShader
    {
        std::vector<Binding> textures;
        bool has_light;

        std::optional<std::string> ambient;
        std::optional<std::string> diffuse;
        std::optional<std::string> specular;
        std::optional<std::string> shininess;

        std::vector<DefaultTexture> default_textures;
    };

    JSON_PARSE_FUNC(MaterialShader);
}
