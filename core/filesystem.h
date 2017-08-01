//
// Created by gustav on 2017-08-01.
//

#ifndef EUPHORIA_FILESYSTEM_H
#define EUPHORIA_FILESYSTEM_H

#include <string>
#include <map>

// todo: add path class

class FileSystem {
 public:
  FileSystem();
  virtual ~FileSystem();

  // todo: load file to string should be a util and this should load file to memory
  // todo: need to support paging too
  virtual bool LoadFileToString(const std::string& path, std::string* source) = 0;

  // todo: support different roots such as real file system, zip/container file etc
  // todo: support encryption
  // todo: support listing/enumerating files
};

class CatalogFileSystem : public FileSystem {
 public:
  explicit CatalogFileSystem(FileSystem* backing_filesys);

  void RegisterFile(const std::string& path, const std::string& content);

  bool LoadFileToString(const std::string& path, std::string* source) override;
 private:
  FileSystem* backing_filesystem_;
  std::map<std::string, std::string> catalog_;
};

#endif //EUPHORIA_FILESYSTEM_H
