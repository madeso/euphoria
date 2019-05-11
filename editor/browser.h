#ifndef EUPHORIA_BROWSER_H
#define EUPHORIA_BROWSER_H

#include <vector>
#include <string>

#include "core/vfs.h"

struct FileBrowser
{
  std::string             current_folder;
  int                     selected_file = -1;
  std::vector<vfs::ListedFile> files;
  std::string             filter      = "";
  vfs::FileSystem*             file_system = nullptr;

  explicit FileBrowser(vfs::FileSystem* fs);

  std::string
  GetSelectedFile();

  void
  SelectFile(const std::string& p);

  void
  Refresh();

  bool
  Run();
};


#endif  // EUPHORIA_BROWSER_H
