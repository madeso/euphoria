#pragma once

#include "core/vfs.h"
#include "core/vfs_path.h"

namespace euphoria::core::vfs
{
    struct ReadRootImageGenerator : ReadRoot
    {
        static void
        add(FileSystem* fs, const DirPath& base);

        std::shared_ptr<MemoryChunk>
        read_file(const FilePath& path) override;

        void
        add_description(std::vector<std::string>* strings) override;

        explicit ReadRootImageGenerator(const DirPath& base);

        FileList
        list_files(const DirPath& path) override;

        DirPath base;
    };

}
