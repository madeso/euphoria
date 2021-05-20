#ifndef T3D_TILELIBRARY_H
#define T3D_TILELIBRARY_H

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
    struct Tile
    {
        Tile();
        ~Tile();

        core::vfs::file_path path;
        core::aabb  aabb;
        std::string name;

        std::shared_ptr<render::compiled_mesh> mesh;
    };


    struct TileLibrary
    {
        TileLibrary(core::vfs::file_system* fs);


        std::shared_ptr<Tile>
        FirstTile();


        bool
        ImGuiList(std::shared_ptr<Tile>* selected_tile);


        void
        AddFile
        (
            const core::vfs::file_path& path,
            render::material_shader_cache* shader_cache,
            render::texture_cache* texture_cache
        );


        core::vfs::file_system* file_system;
        std::vector<std::shared_ptr<Tile>> tiles;
    };

}  // namespace euphoria::t3d

#endif  // T3D_TILELIBRARY_H
