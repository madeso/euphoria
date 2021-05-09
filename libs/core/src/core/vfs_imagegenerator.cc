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
    FileSystemImageGenerator::AddRoot(
            FileSystem*        fs,
            const DirPath& base)
    {
        auto root = std::make_shared<FileSystemImageGenerator>(base);
        fs->AddReadRoot(root);
    }


    std::shared_ptr<memory_chunk>
    FileSystemImageGenerator::ReadFile(const FilePath& path)
    {
        const auto [dir, command] = path.SplitDirectoriesAndFile();
        if(dir != base_)
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
    FileSystemImageGenerator::Describe(std::vector<std::string>* strings)
    {
        strings->emplace_back(string_builder() << base_ << "<color>");
    }


    FileSystemImageGenerator::FileSystemImageGenerator(const DirPath& base)
        : base_(base)
    {
        ASSERT(!base.ContainsRelative());
    }


    FileList
    FileSystemImageGenerator::ListFiles(const DirPath& path)
    {
        ASSERT(!path.ContainsRelative());

        FileList ret;

        if(base_ != DirPath::FromRoot())
        {
            if(path == base_.GetParentDirectory())
            {
                ret.Add(base_.GetDirectoryName() + "/", true, false);
            }
        }

        if(path == base_)
        {
            const auto names = enum_to_string<color>();
            for(const auto& n: names)
            {
                ret.Add(n, true, true);
            }
        }

        return ret;
    }

}
