#include "core/filesystemimagegenerator.h"

#include "core/image.h"
#include "core/draw.h"
#include "core/stringutils.h"
#include "core/log.h"
#include "core/path.h"

LOG_SPECIFY_DEFAULT_LOGGER("filesystem.image-generator")

void
FileSystemImageGenerator::AddRoot(FileSystem* fs, const std::string& base)
{
  auto root = std::make_shared<FileSystemImageGenerator>(base);
  fs->AddReadRoot(root);
}

std::shared_ptr<MemoryChunk>
FileSystemImageGenerator::ReadFile(const std::string& path)
{
  if(!StartsWith(path, base_))
  {
    return MemoryChunk::Null();
  }

  const auto command = path.substr(base_.length());

  const auto color_name = ToLower(command);

  if(!color::IsValidLowerCase(color_name))
  {
    LOG_WARN("Invalid color name: " << color_name << " for path " << path);
    return MemoryChunk::Null();
  }

  const auto color = color::GetColorFromLowerCaseString(color_name);

  Image image;
  image.SetupNoAlphaSupport(128, 128);
  Draw drawer{&image};
  drawer.Clear(color);
  return image.Write(ImageWriteFormat::PNG);
}

std::string
FileSystemImageGenerator::Describe()
{
  return Str() << base_ << "/<color>";
}

FileSystemImageGenerator::FileSystemImageGenerator(const std::string& base)
    : base_(base)
{
  if(!EndsWith(base, "/"))
  {
    base_ = base + "/";
  }
}

std::vector<std::string>
FileSystemImageGenerator::ListFiles(const Path& path)
{
  const auto self = Path::FromDirectory(base_);

  std::vector<std::string> ret;

  if(path == self.GetParentDirectory())
  {
    ret.emplace_back(self.GetDirectoryName());
  }

  if(path == self)
  {
    ret = color::ListAllColorNames();
  }

  return ret;
}
