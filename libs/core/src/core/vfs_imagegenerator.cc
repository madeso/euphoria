#include "core/vfs_imagegenerator.h"

#include "core/image.h"
#include "core/image_draw.h"
#include "core/stringutils.h"
#include "core/stringmerger.h"
#include "log/log.h"
#include "core/vfs_path.h"

namespace euphoria::core::vfs
{
    void
    ReadRootImageGenerator::add
    (
        FileSystem* fs,
        const DirPath& base
    )
    {
        auto root = std::make_shared<ReadRootImageGenerator>(base);
        fs->add_read_root(root);
    }


    std::shared_ptr<MemoryChunk>
    ReadRootImageGenerator::read_file(const FilePath& path)
    {
        const auto [dir, command] = path.split_directories_and_file();
        if(dir != base)
        {
            return MemoryChunk::null();
        }

        const auto color_name = to_lower(command);

        const auto found_color = string_to_enum<NamedColor>(color_name);

        if(!found_color.single_match)
        {
            LOG_WARN
            (
                "Invalid color name: {0} for path {1} closest matches are {2}",
                color_name,
                path,
                string_mergers::english_or.merge(enum_to_string(found_color.values))
            );
            return MemoryChunk::null();
        }

        const auto color = found_color.values[0];

        Image image;
        image.setup_no_alpha_support(128, 128);
        clear(&image, {color});
        return image.write(ImageWriteFormat::png);
    }


    void
    ReadRootImageGenerator::add_description(std::vector<std::string>* strings)
    {
        strings->emplace_back("{}<color>"_format(base));
    }


    ReadRootImageGenerator::ReadRootImageGenerator(const DirPath& b)
        : base(b)
    {
        ASSERT(!base.contains_relative());
    }


    FileList
    ReadRootImageGenerator::list_files(const DirPath& path)
    {
        ASSERT(!path.contains_relative());

        FileList ret;

        if(base != DirPath::from_root())
        {
            if(path == base.get_parent_directory())
            {
                ret.add(base.get_directory_name() + "/", true, false);
            }
        }

        if(path == base)
        {
            const auto names = enum_to_string<NamedColor>();
            for(const auto& n: names)
            {
                ret.add(n, true, true);
            }
        }

        return ret;
    }

}
