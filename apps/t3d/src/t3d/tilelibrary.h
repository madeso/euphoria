#pragma once

#include "core/aabb.h"
#include "core/vfs_path.h"

#include "render/compiledmesh.h"

#include <string>
#include <vector>


namespace euphoria::core::vfs
{
    struct file_system;
}

namespace euphoria::t3d
{
    struct tile
    {
        tile();
        ~tile();

        core::vfs::file_path path;
        core::aabb aabb;
        std::string name;

        std::shared_ptr<render::compiled_mesh> mesh;
    };


    struct tile_library
    {
        tile_library(core::vfs::file_system* fs);


        std::shared_ptr<tile>
        get_first_tile();


        bool
        run_imgui_list(std::shared_ptr<tile>* selected_tile);


        void
        add_file
        (
            const core::vfs::file_path& path,
            render::material_shader_cache* shader_cache,
            render::texture_cache* texture_cache
        );


        core::vfs::file_system* file_system;
        std::vector<std::shared_ptr<tile>> tiles;
    };

}
