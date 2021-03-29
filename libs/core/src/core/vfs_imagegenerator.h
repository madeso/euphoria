#pragma once

#include "core/vfs.h"
#include "core/vfs_path.h"

namespace euphoria::core::vfs
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

}
