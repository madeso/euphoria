#include "t3d/tilelibrary.h"

#include "core/os.h"
#include "core/mesh.h"
#include "core/vfs.h"
#include "core/log.h"
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


TileLibrary::TileLibrary(core::vfs::FileSystem* fs) : file_system(fs)
{}


void
TileLibrary::AddDirectory(
    const std::string&   directory_path,
    render::MaterialShaderCache* shader_cache,
    render::TextureCache*        texture_cache)
{
  const auto dir = core::vfs::Path::FromDirectory(directory_path);
  auto files = file_system->ListFiles(dir);
  for(const auto& relative_file : files)
  {
    const auto file = dir.GetFile(relative_file.name);
    const auto file_path = file.GetAbsolutePath();
    if(core::GetExtension(relative_file.name) == ".obj")
    {
      AddFile(
          file_path,
          shader_cache,
          texture_cache);
    }
  }
}

void
TileLibrary::AddFile(
    const std::string&   path,
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
  tile->mesh = CompileMesh(
      loaded_mesh.mesh, shader_cache, texture_cache, core::vfs::Path::FromRoot());
  tiles.push_back(tile);
}

}
