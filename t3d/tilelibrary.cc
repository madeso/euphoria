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

    Tile::Tile() : aabb(core::Aabb::Empty()), path("~/unknown_tile") {}


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
    TileLibrary::AddFile
    (
        const core::vfs::FilePath& path,
        render::MaterialShaderCache* shader_cache,
        render::TextureCache* texture_cache
    )
    {
        const auto loaded_mesh = core::meshes::LoadMesh(file_system, path);
        if(!loaded_mesh.error.empty())
        {
            LOG_WARN("Failed to open {0}: {1}", path, loaded_mesh.error);
            return;
        }

        auto tile  = std::make_shared<Tile>();
        tile->path = path;
        tile->name = path.GetFilenameWithoutExtension();
        tile->aabb = loaded_mesh.mesh.CalculateAabb();
        tile->mesh = CompileMesh
        (
            loaded_mesh.mesh,
            shader_cache,
            texture_cache,
            // todo(Gustav): test with mesh directory iunstead of root?
            core::vfs::DirPath::FromRoot()
        );
        tiles.push_back(tile);
    }

}  // namespace euphoria::t3d
