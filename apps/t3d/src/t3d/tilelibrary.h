#pragma once

#include "core/aabb.h"
#include "core/vfs_path.h"
#include "core/noncopyable.h"

#include "render/compiledmesh.h"

#include <string>
#include <vector>


namespace euphoria::core::vfs
{
    struct FileSystem;
}

namespace euphoria::t3d
{
    struct tile
    {
        tile();
        ~tile();

        NONCOPYABLE(tile);

        core::vfs::FilePath path;
        core::Aabb aabb;
        std::string name;

        std::shared_ptr<render::compiled_mesh> mesh;
    };


    struct tile_library
    {
        tile_library(core::vfs::FileSystem* fs);


        std::shared_ptr<tile>
        get_first_tile();


        bool
        run_imgui_list(std::shared_ptr<tile>* selected_tile);


        void
        add_file
        (
            const core::vfs::FilePath& path,
            render::material_shader_cache* shader_cache,
            render::texture_cache* texture_cache
        );


        core::vfs::FileSystem* file_system;
        std::vector<std::shared_ptr<tile>> tiles;
    };

}
