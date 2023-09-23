#pragma once

#include "io/json.h"


namespace eu::files::mesh
{
    struct Texture
    {
        std::string type;
        std::string path;
    };

    struct Material
    {
        std::string name;
        std::vector<Texture> textures;
    };

    struct Mesh
    {
        std::vector<Material> materials;
        bool diffuse_and_ambient_are_same = true;
    };

    struct Extension
    {
        std::string old_ext;
        std::string new_ext;
    };

    struct Filename
    {
        std::string old_file;
        std::string new_file;
    };

    struct Folder
    {
        std::string texture_override;
        std::vector<Extension> change_extensions;
        std::vector<Filename> change_filenames;
        float scale = 1.0f;
    };

    JSON_PARSE_FUNC(Mesh);
    JSON_PARSE_FUNC(Folder);
}
