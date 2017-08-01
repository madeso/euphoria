#include "core/filesystem.h"

////////////////////////////////////////////////////////////////////////////////

FileSystem::FileSystem() {}
FileSystem::~FileSystem() {}

////////////////////////////////////////////////////////////////////////////////

CatalogFileSystem::CatalogFileSystem(FileSystem* backing_filesys)
    : backing_filesystem_(backing_filesys)
{ }

void CatalogFileSystem::RegisterFile(const std::string& path, const std::string& content)
{
  catalog_.insert(std::make_pair(path, content));
}

bool CatalogFileSystem::LoadFileToString(const std::string& path, std::string* source)
{
  const auto found = catalog_.find(path);

  if(found == catalog_.end()) {
    if(backing_filesystem_) {
      return backing_filesystem_->LoadFileToString(path, source);
    }
    return false;
  }

  if(source) {
    *source = found->second;
  }

  return true;
}
