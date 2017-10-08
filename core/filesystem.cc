#include "core/filesystem.h"

#include <fstream>

#include "core/assert.h"
#include "core/ints.h"
#include "core/os.h"

#include "core/stringutils.h"

////////////////////////////////////////////////////////////////////////////////

FileSystemRoot::~FileSystemRoot() = default;

////////////////////////////////////////////////////////////////////////////////

FileSystem::FileSystem() = default;

FileSystem::~FileSystem() = default;

void
FileSystem::AddRoot(const std::shared_ptr<FileSystemRoot>& root)
{
  roots_.push_back(root);
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
  std::shared_ptr<MemoryChunk> file = MemoryChunk::Alloc(content.length() + 1);
  CopyToMemory(file.get(), &content[0]);
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
  fs->AddRoot(catalog);
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

////////////////////////////////////////////////////////////////////////////////

FileSystemRootFolder::FileSystemRootFolder(std::string folder)
    : folder_(std::move(folder))
{
}

std::shared_ptr<MemoryChunk>
FileSystemRootFolder::ReadFile(const std::string& path)
{
  const std::string& full_path = folder_ + path;
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

void
FileSystemRootFolder::AddRoot(FileSystem* fs, const std::string& folder)
{
  ASSERT(fs);

  const std::string slash = "/";

  const std::string the_folder =
      EndsWith(folder, slash) ? folder : folder + slash;

  auto catalog = std::make_shared<FileSystemRootFolder>(the_folder);

  fs->AddRoot(catalog);
}

void
FileSystemRootFolder::AddRoot(FileSystem* fs)
{
  const std::string folder = GetCurrentDirectory();
  return FileSystemRootFolder::AddRoot(fs, folder);
}
