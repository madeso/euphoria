#ifndef EUPHORIA_DEFAULTFILES_H
#define EUPHORIA_DEFAULTFILES_H

#include <memory>

namespace vfs
{
  class FileSystemRootCatalog;
}

void
SetupDefaultFiles(std::shared_ptr<vfs::FileSystemRootCatalog> catalog);


#endif  // EUPHORIA_DEFAULTFILES_H
