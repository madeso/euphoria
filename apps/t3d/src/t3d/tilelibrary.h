#pragma once

#include "core/aabb.h"
#include "core/vfs_path.h"
#include "core/collisionmesh.h"

#include "render/compiledmesh.h"


namespace euphoria::core::vfs
{
    struct FileSystem;
}

namespace euphoria::t3d
{
    struct Tile
    {
        Tile();
        ~Tile();

        Tile(const Tile& other) = delete;
        void operator=(const Tile&) = delete;
        Tile(Tile&& other) = delete;
        void operator=(Tile&&) = delete;

        core::vfs::FilePath path;
        core::Aabb aabb;
        std::string name;

        core::CollisionMesh collison;
        std::shared_ptr<render::CompiledMesh> mesh;
    };


    struct TileLibrary
    {
        TileLibrary(core::vfs::FileSystem* fs);


        std::shared_ptr<Tile>
        get_first_tile();


        bool
        run_imgui_list(std::shared_ptr<Tile>* selected_tile);


        void
        add_file
        (
            const core::vfs::FilePath& path,
            render::MaterialShaderCache* shader_cache,
            render::TextureCache* texture_cache
        );


        core::vfs::FileSystem* file_system;
        std::vector<std::shared_ptr<Tile>> tiles;
    };

}
