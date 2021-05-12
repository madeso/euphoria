#include "core/vfs_imagegenerator.h"

#include "core/image.h"
#include "core/image_draw.h"
#include "core/stringutils.h"
#include "core/stringmerger.h"
#include "core/log.h"
#include "core/vfs_path.h"

namespace euphoria::core::vfs
{
    LOG_SPECIFY_DEFAULT_LOGGER("filesystem.image-generator")


    void
    read_root_image_generator::add(
            file_system*        fs,
            const dir_path& base)
    {
        auto root = std::make_shared<read_root_image_generator>(base);
        fs->add_read_root(root);
    }


    std::shared_ptr<memory_chunk>
    read_root_image_generator::read_file(const file_path& path)
    {
        const auto [dir, command] = path.split_directories_and_file();
        if(dir != base)
        {
            return memory_chunk::null();
        }

        const auto color_name = to_lower(command);

        const auto found_color = string_to_enum<color>(color_name);

        if(!found_color.single_match)
        {
            LOG_WARN
            (
                "Invalid color name: {0} for path {1} closest matches are {2}",
                color_name,
                path,
                string_mergers::english_or.merge(enum_to_string(found_color.values))
            );
            return memory_chunk::null();
        }

        const auto color = found_color.values[0];

        image image;
        image.setup_no_alpha_support(128, 128);
        clear(&image, {color});
        return image.write(image_write_format::png);
    }


    void
    read_root_image_generator::add_description(std::vector<std::string>* strings)
    {
        strings->emplace_back(string_builder() << base << "<color>");
    }


    read_root_image_generator::read_root_image_generator(const dir_path& base)
        : base(base)
    {
        ASSERT(!base.contains_relative());
    }


    file_list
    read_root_image_generator::list_files(const dir_path& path)
    {
        ASSERT(!path.contains_relative());

        file_list ret;

        if(base != dir_path::from_root())
        {
            if(path == base.get_parent_directory())
            {
                ret.add(base.get_directory_name() + "/", true, false);
            }
        }

        if(path == base)
        {
            const auto names = enum_to_string<color>();
            for(const auto& n: names)
            {
                ret.add(n, true, true);
            }
        }

        return ret;
    }

}
