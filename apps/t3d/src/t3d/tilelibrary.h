#ifndef T3D_TILELIBRARY_H
#define T3D_TILELIBRARY_H

#include "core/aabb.h"
#include "core/vfs_path.h"

#include "render/compiledmesh.h"

#include <string>
#include <vector>


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

        core::vfs::FilePath path;
        core::Aabb  aabb;
        std::string name;

        std::shared_ptr<render::CompiledMesh> mesh;
    };


    struct TileLibrary
    {
        TileLibrary(core::vfs::FileSystem* fs);


        std::shared_ptr<Tile>
        FirstTile();


        bool
        ImGuiList(std::shared_ptr<Tile>* selected_tile);


        void
        AddFile
        (
            const core::vfs::FilePath& path,
            render::MaterialShaderCache* shader_cache,
            render::TextureCache* texture_cache
        );


        core::vfs::FileSystem* file_system;
        std::vector<std::shared_ptr<Tile>> tiles;
    };

}  // namespace euphoria::t3d

#endif  // T3D_TILELIBRARY_H
