#pragma once

#include "io/vfs.h"
#include "io/vfs_path.h"

namespace eu::core
{
    struct ReadRootImageGenerator : io::ReadRoot
    {
        static void
        add(io::FileSystem* fs, const io::DirPath& base);

        std::shared_ptr<MemoryChunk>
        read_file(const io::FilePath& path) override;

        void
        add_description(std::vector<std::string>* strings) override;

        explicit ReadRootImageGenerator(const io::DirPath& base);

        io::FileList
        list_files(const io::DirPath& path) override;

        io::DirPath base;
    };

}
