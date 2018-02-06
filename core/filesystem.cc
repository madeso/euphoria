#include "core/filesystem.h"

#include <fstream>

#include "core/assert.h"
#include "core/ints.h"
#include "core/os.h"
#include "core/stringmerger.h"
#include "core/log.h"

#include "core/stringutils.h"

LOG_SPECIFY_DEFAULT_LOGGER("core.filesystem")

////////////////////////////////////////////////////////////////////////////////

FileSystemReadRoot::~FileSystemReadRoot() = default;

FileSystemWriteRoot::~FileSystemWriteRoot() = default;

////////////////////////////////////////////////////////////////////////////////

FileSystem::FileSystem() = default;

FileSystem::~FileSystem() = default;

void
FileSystem::AddReadRoot(const std::shared_ptr<FileSystemReadRoot>& root)
{
  roots_.push_back(root);
}

void
FileSystem::SetWrite(const std::shared_ptr<FileSystemWriteRoot>& root)
{
  write_ = root;
}

std::shared_ptr<MemoryChunk>
FileSystem::ReadFile(const std::string& path)
{
  for(auto& root : roots_)
  {
    std::shared_ptr<MemoryChunk> file = root->ReadFile(path);
    if(file != nullptr)
    {
      return file;
    }
  }

  return MemoryChunk::Null();
}

void
FileSystem::WriteFile(
    const std::string& path, std::shared_ptr<MemoryChunk> data)
{
  ASSERT(write_);
  write_->WriteFile(path, data);
}

std::string
FileSystem::GetRootsAsString()
{
  std::vector<std::string> ret;
  for(auto& root : roots_)
  {
    const auto desc = root->Describe();
    if(!desc.empty())
    {
      ret.push_back(desc);
    }
  }

  return StringMerger::Array().Generate(ret);
}

bool
FileSystem::ReadFileToString(const std::string& path, std::string* source)
{
  ASSERT(source);

  std::shared_ptr<MemoryChunk> file = ReadFile(path);
  if(file == nullptr)
  {
    return false;
  }

  // data is unsigned char, cast it to char and assume it is a string
  *source = std::string(file->GetData(), file->GetSize() - 1);
  return true;
}

////////////////////////////////////////////////////////////////////////////////

FileSystemRootCatalog::FileSystemRootCatalog() = default;

void
FileSystemRootCatalog::RegisterFileString(
    const std::string& path, const std::string& content)
{
  std::shared_ptr<MemoryChunk> file = MemoryChunkFromText(content);
  RegisterFileData(path, file);
}

void
FileSystemRootCatalog::RegisterFileData(
    const std::string& path, const std::shared_ptr<MemoryChunk>& content)
{
  catalog_.insert(std::make_pair(path, content));
}

std::shared_ptr<FileSystemRootCatalog>
FileSystemRootCatalog::AddRoot(FileSystem* fs)
{
  ASSERT(fs);
  auto catalog = std::make_shared<FileSystemRootCatalog>();
  fs->AddReadRoot(catalog);
  return catalog;
}

std::shared_ptr<MemoryChunk>
FileSystemRootCatalog::ReadFile(const std::string& path)
{
  const auto found = catalog_.find(path);

  if(found == catalog_.end())
  {
    return MemoryChunk::Null();
  }

  return found->second;
}

std::string
FileSystemRootCatalog::Describe()
{
  return StringMerger::Array().Generate(MapToStringVector(catalog_));
}

////////////////////////////////////////////////////////////////////////////////

std::string
CombineFolderAndPath(const std::string& folder, const std::string& path)
{
  return folder + path;
}

std::string
MakeSureEndsWithSlash(const std::string& folder)
{
  const std::string slash = "/";

  const std::string the_folder =
      EndsWith(folder, slash) ? folder : folder + slash;
  return the_folder;
}

////////////////////////////////////////////////////////////////////////////////

FileSystemRootFolder::FileSystemRootFolder(std::string folder)
    : folder_(std::move(folder))
{
}

std::shared_ptr<MemoryChunk>
FileSystemRootFolder::ReadFile(const std::string& path)
{
  const std::string& full_path = CombineFolderAndPath(folder_, path);
  std::ifstream      is(full_path, std::ifstream::binary);
  if(!is)
  {
    return MemoryChunk::Null();
  }

  is.seekg(0, is.end);
  fint64 length = is.tellg();
  is.seekg(0, is.beg);

  if(length <= 0)
  {
    return MemoryChunk::Null();
  }

  auto memory = MemoryChunk::Alloc(length);
  is.read(
      static_cast<char*>(static_cast<void*>(memory->GetData())),
      memory->GetSize());

  return memory;
}

std::string
FileSystemRootFolder::Describe()
{
  return folder_;
}

void
FileSystemRootFolder::AddRoot(FileSystem* fs, const std::string& folder)
{
  ASSERT(fs);

  auto catalog =
      std::make_shared<FileSystemRootFolder>(MakeSureEndsWithSlash(folder));

  fs->AddReadRoot(catalog);
}

void
FileSystemRootFolder::AddRoot(FileSystem* fs)
{
  const std::string folder = GetCurrentDirectory();
  return FileSystemRootFolder::AddRoot(fs, folder);
}

////////////////////////////////////////////////////////////////////////////////

FileSystemWriteFolder::FileSystemWriteFolder(const std::string& f)
    : folder(MakeSureEndsWithSlash(f))
{
}

void
FileSystemWriteFolder::WriteFile(
    const std::string& path, std::shared_ptr<MemoryChunk> data)
{
  const auto    full_path = CombineFolderAndPath(folder, path);
  std::ofstream os(full_path, std::ifstream::binary);
  if(!os.good())
  {
    LOG_ERROR("Failed to open file " << full_path);
    return;
  }
  os.write(data->GetData(), data->GetSize());
}
