#include "t3d/tilelibrary.h"

#include "core/os.h"
#include "core/mesh.h"
#include "core/vfs.h"
#include "core/log.h"
#include "core/path.h"
#include "core/vfs_path.h"

namespace euphoria::t3d
{

LOG_SPECIFY_DEFAULT_LOGGER("tile_library")

Tile::Tile()
    : aabb(core::Aabb::Empty())
{
}

Tile::~Tile()
{
}

void
TileLibrary::AddDirectory(
    const std::string&   directory_path,
    render::MaterialShaderCache* shader_cache,
    render::TextureCache*        texture_cache)
{
  const auto directory = core::ListDirectory(directory_path);
  for(const auto& file : directory.files)
  {
    if(core::GetExtension(file) == ".obj")
    {
      AddFile(
          directory_path,
          core::JoinPath(directory_path, file),
          shader_cache,
          texture_cache);
    }
  }
}

void
TileLibrary::AddFile(
    const std::string&   current_directory,
    const std::string&   path,
    render::MaterialShaderCache* shader_cache,
    render::TextureCache*        texture_cache)
{
  core::vfs::FileSystem file_system;
  core::vfs::FileSystemRootFolder::AddRoot(&file_system, current_directory);
  const auto loaded_mesh = core::meshes::LoadMesh(&file_system, path);
  if(!loaded_mesh.error.empty())
  {
    LOG_WARN("Failed to open " << path << ": " << loaded_mesh.error);
    return;
  }

  auto tile  = std::make_shared<Tile>();
  tile->path = path;
  tile->name = core::GetFileNameWithoutExtension(path);
  tile->aabb = loaded_mesh.mesh.CalculateAabb();
  tile->mesh = CompileMesh(
      loaded_mesh.mesh, shader_cache, texture_cache, core::vfs::Path::FromRoot());
  tiles.push_back(tile);
}

}
