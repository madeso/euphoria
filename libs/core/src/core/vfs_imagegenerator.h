#pragma once

#include "core/vfs.h"
#include "core/vfs_path.h"

namespace euphoria::core::vfs
{
    struct read_root_image_generator : read_root
    {
        static void
        add(file_system* fs, const dir_path& base);

        std::shared_ptr<memory_chunk>
        read_file(const file_path& path) override;

        void
        add_description(std::vector<std::string>* strings) override;

        explicit read_root_image_generator(const dir_path& base);

        file_list
        list_files(const dir_path& path) override;

        dir_path base;
    };

}
