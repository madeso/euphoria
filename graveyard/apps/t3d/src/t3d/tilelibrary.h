#pragma once

#include "base/aabb.h"
#include "io/vfs_path.h"
#include "core/collisionmesh.h"

#include "render/compiledmesh.h"


namespace eu::io
{
    struct FileSystem;
}

namespace eu::t3d
{
    struct Tile
    {
        Tile();
        ~Tile();

        Tile(const Tile& other) = delete;
        void operator=(const Tile&) = delete;
        Tile(Tile&& other) = delete;
        void operator=(Tile&&) = delete;

        io::FilePath path;
        Aabb aabb;
        std::string name;

        core::CollisionMesh collison;
        std::shared_ptr<render::CompiledMesh> mesh;
    };


    struct TileLibrary
    {
        TileLibrary(io::FileSystem* fs);


        std::shared_ptr<Tile>
        get_first_tile();


        bool
        run_imgui_list(std::shared_ptr<Tile>* selected_tile);


        void
        add_file
        (
            const io::FilePath& path,
            render::MaterialShaderCache* shader_cache,
            render::TextureCache* texture_cache
        );


        io::FileSystem* file_system;
        std::vector<std::shared_ptr<Tile>> tiles;
    };

}
