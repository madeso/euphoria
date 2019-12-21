#include "t3d/tilelibrary.h"


#include "imgui/imgui.h"

#include "core/os.h"
#include "core/mesh.h"
#include "core/vfs.h"
#include "core/log.h"
#include "core/vfs_path.h"


namespace euphoria::t3d
{
    LOG_SPECIFY_DEFAULT_LOGGER("tile_library")

    Tile::Tile() : aabb(core::Aabb::Empty()) {}


    Tile::~Tile() {}


    TileLibrary::TileLibrary(core::vfs::FileSystem* fs) : file_system(fs) {}


    std::shared_ptr<Tile>
    TileLibrary::FirstTile()
    {
        if(tiles.empty()) { return nullptr; }
        else return tiles[0];
    }


    bool
    TileLibrary::ImGuiList(std::shared_ptr<Tile>* selected_tile)
    {
        if(tiles.empty()) return false;

        bool tile_has_changed = false;

        ImGui::ListBoxHeader("Tiles");
        for(auto tile: tiles)
        {
            std::string display = core::Str {}
                << tile->name << ": "
                << tile->aabb.GetSize();
            if(ImGui::Selectable(
                        display.c_str(),
                        (*selected_tile)->mesh == tile->mesh))
            {
                *selected_tile = tile;
                tile_has_changed = true;
            }
        }
        ImGui::ListBoxFooter();

        return tile_has_changed;
    }


    void
    TileLibrary::AddDirectory(
            const std::string&           directory_path,
            render::MaterialShaderCache* shader_cache,
            render::TextureCache*        texture_cache)
    {
        const auto dir   = core::vfs::Path::FromDirectory(directory_path);
        auto       files = file_system->ListFiles(dir);
        for(const auto& relative_file: files)
        {
            const auto file      = dir.GetFile(relative_file.name);
            const auto file_path = file.GetAbsolutePath();
            if(core::GetExtension(relative_file.name) == ".obj")
            {
                AddFile(file_path, shader_cache, texture_cache);
            }
        }
    }


    void
    TileLibrary::AddFile(
            const std::string&           path,
            render::MaterialShaderCache* shader_cache,
            render::TextureCache*        texture_cache)
    {
        const auto loaded_mesh = core::meshes::LoadMesh(file_system, path);
        if(!loaded_mesh.error.empty())
        {
            LOG_WARN("Failed to open " << path << ": " << loaded_mesh.error);
            return;
        }

        auto tile  = std::make_shared<Tile>();
        tile->path = path;
        tile->name = core::GetFileNameWithoutExtension(path);
        tile->aabb = loaded_mesh.mesh.CalculateAabb();
        tile->mesh = CompileMesh
        (
            loaded_mesh.mesh,
            shader_cache,
            texture_cache,
            core::vfs::Path::FromRoot()
        );
        tiles.push_back(tile);
    }

}  // namespace euphoria::t3d
