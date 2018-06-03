#include "core/filesystemdefaultshaders.h"

#include "core/stringutils.h"
#include "core/log.h"
#include "core/path.h"

// LOG_SPECIFY_DEFAULT_LOGGER("filesystem.default-shaders")

void
FileSystemDefaultShaders::AddRoot(FileSystem* fs, const std::string& base)
{
  auto root = std::make_shared<FileSystemDefaultShaders>(base);
  fs->AddReadRoot(root);
}

std::shared_ptr<MemoryChunk>
FileSystemDefaultShaders::ReadFile(const std::string& path)
{
  if(!StartsWith(path, base_))
  {
    return MemoryChunk::Null();
  }
  const auto command     = path.substr(base_.length());
  const auto shader_name = ToLower(command);

  if(shader_name == "sprite.vert")
  {
    return MemoryChunkFromText(R"STRING(
#version 330 core

in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

uniform vec4 color;
uniform vec4 region;

void main()
{
    TexCoords = vec2(region.x + vertex.z * (region.y - region.x),
                     region.z + vertex.w * (region.w - region.z));
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}
)STRING");
  }
  else if(shader_name == "sprite.frag")
  {
    return MemoryChunkFromText(R"STRING(
#version 330 core

in vec2 TexCoords;
out vec4 outColor;

uniform sampler2D image;
uniform vec4 color;

void main()
{
    outColor = color * texture(image, TexCoords);
}
)STRING");
  }

  return MemoryChunk::Null();
}

std::string
FileSystemDefaultShaders::Describe()
{
  return Str() << base_ << "/<default-shader>";
}

FileSystemDefaultShaders::FileSystemDefaultShaders(const std::string& base)
    : base_(base)
{
  if(!EndsWith(base, "/"))
  {
    base_ = base + "/";
  }
}

std::vector<ListedFile>
FileSystemDefaultShaders::ListFiles(const Path& path)
{
  const auto self = Path::FromDirectory(base_);

  std::vector<ListedFile> ret;

  if(path == self.GetParentDirectory())
  {
    ret.emplace_back(self.GetDirectoryName(), true);
  }

  if(path == self)
  {
    ret.emplace_back("sprite.vert", true);
    ret.emplace_back("sprite.frag", true);
  }

  return ret;
}
