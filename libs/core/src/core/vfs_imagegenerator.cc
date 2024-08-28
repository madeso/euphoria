#include "core/vfs_imagegenerator.h"

#include "core/image.h"
#include "base/stringutils.h"
#include "base/stringmerger.h"
#include "log/log.h"
#include "io/vfs_path.h"

namespace eu::core
{
    void
    ReadRootImageGenerator::add
    (
        io::FileSystem* fs,
        const io::DirPath& base
    )
    {
        auto root = std::make_shared<ReadRootImageGenerator>(base);
        fs->add_read_root(root);
    }


    std::shared_ptr<MemoryChunk>
    ReadRootImageGenerator::read_file(const io::FilePath& path)
    {
        // todo(Gustav): replace this with proper 1x1 pixel images
        return MemoryChunk::create_null();
    }


    void
    ReadRootImageGenerator::add_description(std::vector<std::string>* strings)
    {
        strings->emplace_back(fmt::format("{}<color>", base));
    }


    ReadRootImageGenerator::ReadRootImageGenerator(const io::DirPath& b)
        : base(b)
    {
        ASSERT(!base.contains_relative());
    }


    io::FileList
    ReadRootImageGenerator::list_files(const io::DirPath& path)
    {
        ASSERT(!path.contains_relative());

        io::FileList ret;

        if(base != io::DirPath::from_root())
        {
            if(path == base.get_parent_directory())
            {
                ret.add(base.get_name() + "/", true, false);
            }
        }

        if(path == base)
        {
            const auto names = get_all_names_from_enum<NamedColor>();
            for(const auto& n: names)
            {
                ret.add(n, true, true);
            }
        }

        return ret;
    }

}
