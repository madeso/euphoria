#ifndef EUPHORIA_FILESYSTEMIMAGEGENERATOR_H
#define EUPHORIA_FILESYSTEMIMAGEGENERATOR_H

#include "core/filesystem.h"


class FileSystemImageGenerator : public FileSystemRoot
{
 public:
  static void
  AddRoot(FileSystem* fs, const std::string& base);

  std::shared_ptr<MemoryChunk>
  ReadFile(const std::string& path) override;

  std::string
  Describe() override;

  explicit FileSystemImageGenerator(const std::string& base);

 private:
  std::string base_;
};


#endif  // EUPHORIA_FILESYSTEMIMAGEGENERATOR_H
