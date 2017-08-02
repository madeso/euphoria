#include "core/filesystem.h"
#include "core/assert.h"

////////////////////////////////////////////////////////////////////////////////

FileSystemRoot::~FileSystemRoot() {}

////////////////////////////////////////////////////////////////////////////////

FileSystem::FileSystem() {}

FileSystem::~FileSystem() {}

void FileSystem::AddRoot(std::shared_ptr<FileSystemRoot> root)
{
  roots_.push_back(root);
}

std::shared_ptr<MemoryChunk> FileSystem::ReadFile(const std::string& path)
{
  for(auto root : roots_) {
    std::shared_ptr<MemoryChunk> file = root->ReadFile(path);
    if(file != nullptr) {
      return file;
    }
  }

  return MemoryChunk::Null();
}

bool FileSystem::ReadFileToString(const std::string& path, std::string* source) {
  Assert(source);

  std::shared_ptr<MemoryChunk> file = ReadFile(path);
  if(file == nullptr) {
    return false;
  }

  // data is unsigned char, cast it to char and assume it is a string
  *source = std::string(reinterpret_cast<char*>(file->GetData()), file->GetSize()-1);
  return true;
}

////////////////////////////////////////////////////////////////////////////////

FileSystemRootCatalog::FileSystemRootCatalog() {}

void FileSystemRootCatalog::RegisterFileString(const std::string& path, const std::string& content)
{
  std::shared_ptr<MemoryChunk> file = MemoryChunk::Alloc(content.length() + 1);
  CopyToMemory(file.get(), &content[0]);
  RegisterFileData(path, file);
}

void FileSystemRootCatalog::RegisterFileData(const std::string& path, const std::shared_ptr<MemoryChunk> content)
{
  catalog_.insert(std::make_pair(path, content));
}

std::shared_ptr<FileSystemRootCatalog> FileSystemRootCatalog::AddRoot(FileSystem* fs) {
  Assert(fs);
  auto catalog = std::make_shared<FileSystemRootCatalog>();
  fs->AddRoot(catalog);
  return catalog;
}

std::shared_ptr<MemoryChunk> FileSystemRootCatalog::ReadFile(const std::string& path)
{
  const auto found = catalog_.find(path);

  if(found == catalog_.end()) {
    return MemoryChunk::Null();
  }

  return found->second;
}
