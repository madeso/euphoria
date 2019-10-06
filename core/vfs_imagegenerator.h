#ifndef EUPHORIA_FILESYSTEMIMAGEGENERATOR_H
#define EUPHORIA_FILESYSTEMIMAGEGENERATOR_H

#include "core/vfs.h"

namespace euphoria::core
{
    namespace vfs
    {
        class FileSystemImageGenerator : public FileSystemReadRoot
        {
            public:
            static void
            AddRoot(FileSystem* fs, const std::string& base);

            std::shared_ptr<MemoryChunk>
            ReadFile(const std::string& path) override;

            std::string
            Describe() override;

            explicit FileSystemImageGenerator(const std::string& base);

            FileList
            ListFiles(const Path& path) override;

            private:
            std::string base_;
        };

    }  // namespace vfs

}  // namespace euphoria::core

#endif  // EUPHORIA_FILESYSTEMIMAGEGENERATOR_H
