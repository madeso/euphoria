#ifndef T3D_TILELIBRARY_H
#define T3D_TILELIBRARY_H

#include <string>
#include <vector>

#include "core/aabb.h"
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

        std::string path;
        core::Aabb  aabb;
        std::string name;

        std::shared_ptr<render::CompiledMesh> mesh;
    };


    struct TileLibrary
    {
        core::vfs::FileSystem* file_system;

        TileLibrary(core::vfs::FileSystem* fs);

        void
        AddDirectory(
                const std::string&           directory_path,
                render::MaterialShaderCache* shader_cache,
                render::TextureCache*        texture_cache);

        void
        AddFile(const std::string&           path,
                render::MaterialShaderCache* shader_cache,
                render::TextureCache*        texture_cache);

        std::vector<std::shared_ptr<Tile>> tiles;
    };

}  // namespace euphoria::t3d

#endif  // T3D_TILELIBRARY_H
