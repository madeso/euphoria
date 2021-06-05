#include "t3d/tilelibrary.h"


#include "imgui/imgui.h"

#include "core/os.h"
#include "core/mesh.h"
#include "core/vfs.h"
#include "core/log.h"
#include "core/vfs_path.h"


namespace euphoria::t3d
{
    tile::tile()
        : path("~/unknown_tile")
        , aabb(core::aabb::Empty())
    {
    }


    tile::~tile() {}


    tile_library::tile_library(core::vfs::file_system* fs) : file_system(fs) {}


    std::shared_ptr<tile>
    tile_library::get_first_tile()
    {
        if(tiles.empty()) { return nullptr; }
        else return tiles[0];
    }


    bool
    tile_library::run_imgui_list(std::shared_ptr<tile>* selected_tile)
    {
        if(tiles.empty()) return false;

        bool tile_has_changed = false;

        ImGui::BeginListBox("Tiles");
        for(auto tile: tiles)
        {
            std::string display = core::string_builder {}
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
    tile_library::add_file
    (
        const core::vfs::file_path& path,
        render::material_shader_cache* shader_cache,
        render::texture_cache* texture_cache
    )
    {
        const auto loaded_mesh = core::meshes::load_mesh(file_system, path);
        if(!loaded_mesh.error.empty())
        {
            LOG_WARN("Failed to open {0}: {1}", path, loaded_mesh.error);
            return;
        }

        auto tile = std::make_shared<t3d::tile>();
        tile->path = path;
        tile->name = path.get_filename_without_extension();
        tile->aabb = loaded_mesh.loaded_mesh.calculate_aabb();
        tile->mesh = compile_mesh
        (
            loaded_mesh.loaded_mesh,
            shader_cache,
            texture_cache,
            // todo(Gustav): test with mesh directory instead of root?
            core::vfs::dir_path::from_root(),
            path.path
        );
        tiles.push_back(tile);
    }

}
