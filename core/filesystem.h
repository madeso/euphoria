#ifndef EUPHORIA_FILESYSTEM_H
#define EUPHORIA_FILESYSTEM_H

#include "core/memorychunk.h"

#include <string>
#include <map>
#include <memory>
#include <vector>

// todo: add path class

class FileSystemRoot
{
 public:
  virtual ~FileSystemRoot();

  virtual std::shared_ptr<MemoryChunk>
  ReadFile(const std::string& path) = 0;
};

class FileSystem
{
 public:
  FileSystem();
  ~FileSystem();

  void
  AddRoot(std::shared_ptr<FileSystemRoot> root);

  std::shared_ptr<MemoryChunk>
  ReadFile(const std::string& path);

  // todo: need to support paging too
  bool
  ReadFileToString(const std::string& path, std::string* source);

  // todo: support different roots such as real file system, zip/container file
  // etc
  // todo: support encryption
  // todo: support listing/enumerating files

 private:
  std::vector<std::shared_ptr<FileSystemRoot>> roots_;
};

class FileSystemRootCatalog : public FileSystemRoot
{
 public:
  FileSystemRootCatalog();

  void
  RegisterFileString(const std::string& path, const std::string& content);
  void
  RegisterFileData(const std::string&                 path,
                   const std::shared_ptr<MemoryChunk> content);

  static std::shared_ptr<FileSystemRootCatalog>
  AddRoot(FileSystem* fs);

  std::shared_ptr<MemoryChunk>
  ReadFile(const std::string& path) override;

 private:
  std::map<std::string, std::shared_ptr<MemoryChunk>> catalog_;
};

class FileSystemRootFolder : public FileSystemRoot
{
 public:
  explicit FileSystemRootFolder(const std::string& folder);

  std::shared_ptr<MemoryChunk>
  ReadFile(const std::string& path) override;

  static void
  AddRoot(FileSystem* fs, const std::string& folder);
  static void
  AddRoot(FileSystem* fs);

 private:
  std::string folder_;
};


#endif  // EUPHORIA_FILESYSTEM_H
