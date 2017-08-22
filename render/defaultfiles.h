#ifndef EUPHORIA_DEFAULTFILES_H
#define EUPHORIA_DEFAULTFILES_H

#include <memory>

class FileSystemRootCatalog;

void
SetupDefaultFiles(std::shared_ptr<FileSystemRootCatalog> catalog);


#endif  // EUPHORIA_DEFAULTFILES_H
