#include "core/vfs_imagegenerator.h"

#include "core/image.h"
#include "core/draw.h"
#include "core/stringutils.h"
#include "core/stringmerger.h"
#include "core/log.h"
#include "core/vfs_path.h"

namespace euphoria::core
{
    namespace vfs
    {
        LOG_SPECIFY_DEFAULT_LOGGER("filesystem.image-generator")

        void
        FileSystemImageGenerator::AddRoot(
                FileSystem*        fs,
                const std::string& base)
        {
            auto root = std::make_shared<FileSystemImageGenerator>(base);
            fs->AddReadRoot(root);
        }

        std::shared_ptr<MemoryChunk>
        FileSystemImageGenerator::ReadFile(const std::string& path)
        {
            if(!StartsWith(path, base_))
            {
                return MemoryChunk::Null();
            }

            const auto command = path.substr(base_.length());

            const auto color_name = ToLower(command);

            const auto found_color = StringToEnum<Color>(color_name);

            if(!found_color.single_match)
            {
                LOG_WARN(
                        "Invalid color name: "
                        << color_name << " for path " << path
                        << " closest matches are "
                        << StringMerger::EnglishOr().Generate(
                                   EnumToString(found_color.values)));
                return MemoryChunk::Null();
            }

            const auto color = found_color.values[0];

            Image image;
            image.SetupNoAlphaSupport(128, 128);
            Draw drawer {&image};
            drawer.Clear(color);
            return image.Write(ImageWriteFormat::PNG);
        }

        std::string
        FileSystemImageGenerator::Describe()
        {
            return Str() << base_ << "/<color>";
        }

        FileSystemImageGenerator::FileSystemImageGenerator(
                const std::string& base)
            : base_(base)
        {
            if(!EndsWith(base, "/"))
            {
                base_ = base + "/";
            }
        }

        FileList
        FileSystemImageGenerator::ListFiles(const Path& path)
        {
            const auto self = Path::FromDirectory(base_);

            FileList ret;

            if(path == self.GetParentDirectory())
            {
                ret.Add(self.GetDirectoryName(), true);
            }

            if(path == self)
            {
                const auto names = EnumToString<Color>();
                for(const auto& n: names)
                {
                    ret.Add(n, true);
                }
            }

            return ret;
        }

    }  // namespace vfs

}  // namespace euphoria::core
