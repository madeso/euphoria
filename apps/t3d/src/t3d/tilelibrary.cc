#include "t3d/tilelibrary.h"


#include "imgui/imgui.h"

#include "core/os.h"
#include "core/mesh.h"
#include "core/vfs.h"
#include "core/log.h"
#include "core/vfs_path.h"


namespace euphoria::t3d
{
    Tile::Tile()
        : path("~/unknown_tile")
        , aabb(core::Aabb::create_empty())
    {
    }


    Tile::~Tile() = default;


    TileLibrary::TileLibrary(core::vfs::FileSystem* fs) : file_system(fs) {}


    std::shared_ptr<Tile>
    TileLibrary::get_first_tile()
    {
        if(tiles.empty()) { return nullptr; }
        else { return tiles[0]; }
    }


    bool
    TileLibrary::run_imgui_list(std::shared_ptr<Tile>* selected_tile)
    {
        if(tiles.empty()) { return false; }

        bool tile_has_changed = false;

        ImGui::BeginListBox("Tiles");
        for(auto tile: tiles)
        {
            std::string display = core::StringBuilder {}
                << tile->name << ": "
                << tile->aabb.get_size();
            if(ImGui::Selectable(display.c_str(), (*selected_tile)->mesh == tile->mesh))
            {
                *selected_tile = tile;
                tile_has_changed = true;
            }
        }
        ImGui::EndListBox();

        return tile_has_changed;
    }


    void
    TileLibrary::add_file
    (
        const core::vfs::FilePath& path,
        render::MaterialShaderCache* shader_cache,
        render::TextureCache* texture_cache
    )
    {
        const auto loaded_mesh = core::meshes::load_mesh(file_system, path);
        if(!loaded_mesh.error.empty())
        {
            LOG_WARN("Failed to open {0}: {1}", path, loaded_mesh.error);
            return;
        }

        auto tile = std::make_shared<t3d::Tile>();
        tile->path = path;
        tile->name = path.get_filename_without_extension();
        tile->aabb = loaded_mesh.loaded_mesh.calculate_aabb();
        tile->mesh = compile_mesh
        (
            loaded_mesh.loaded_mesh,
            shader_cache,
            texture_cache,
            // todo(Gustav): test with mesh directory instead of root?
            core::vfs::DirPath::from_root(),
            path.path
        );
        tiles.push_back(tile);
    }

}
