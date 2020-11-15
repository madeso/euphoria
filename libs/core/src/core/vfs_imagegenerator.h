#ifndef EUPHORIA_FILESYSTEMIMAGEGENERATOR_H
#define EUPHORIA_FILESYSTEMIMAGEGENERATOR_H

#include "core/vfs.h"
#include "core/vfs_path.h"

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystemImageGenerator : public FileSystemReadRoot
        {
            static void
            AddRoot(FileSystem* fs, const DirPath& base);

            std::shared_ptr<MemoryChunk>
            ReadFile(const FilePath& path) override;

            void
            Describe(std::vector<std::string>* strings) override;

            explicit FileSystemImageGenerator(const DirPath& base);

            FileList
            ListFiles(const DirPath& path) override;

        private:
            DirPath base_;
        };

    }  // namespace vfs

}  // namespace euphoria::core

#endif  // EUPHORIA_FILESYSTEMIMAGEGENERATOR_H
