#include "t3d/tilelibrary.h"


#include "imgui/imgui.h"

#include "base/os.h"
#include "core/mesh.h"
#include "io/vfs.h"
#include "log/log.h"
#include "io/vfs_path.h"


namespace eu::t3d
{
    Tile::Tile()
        : path("~/unknown_tile")
        , aabb(Aabb::create_empty())
    {
    }


    Tile::~Tile() = default;


    TileLibrary::TileLibrary(io::FileSystem* fs)
        : file_system(fs)
    {
    }


    std::shared_ptr<Tile>
    TileLibrary::get_first_tile()
    {
        if(tiles.empty())
        {
            return nullptr;
        }
        else
        {
            return tiles[0];
        }
    }


    bool
    TileLibrary::run_imgui_list(std::shared_ptr<Tile>* selected_tile)
    {
        if(tiles.empty()) { return false; }

        bool tile_has_changed = false;

        ImGui::BeginListBox("Tiles");
        for(auto& tile: tiles)
        {
            std::string display = fmt::format("{}: {}", tile->name, tile->aabb.get_size());
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
        const io::FilePath& path,
        render::MaterialShaderCache* shader_cache,
        render::TextureCache* texture_cache
    )
    {
        const auto loaded_mesh_result = core::meshes::load_mesh(file_system, path);
        if(!loaded_mesh_result.error.empty())
        {
            LOG_WARN("Failed to open {0}: {1}", path, loaded_mesh_result.error);
            return;
        }

        auto tile = std::make_shared<t3d::Tile>();
        tile->path = path;
        tile->name = path.get_filename_without_extension();
        tile->aabb = loaded_mesh_result.loaded_mesh.calc_aabb();
        tile->collison.add(loaded_mesh_result.loaded_mesh);
        tile->mesh = compile_mesh
        (
            loaded_mesh_result.loaded_mesh,
            shader_cache,
            texture_cache,
            // todo(Gustav): test with mesh directory instead of root?
            io::DirPath::from_root(),
            path.path
        );
        tiles.push_back(tile);
    }

}
