#ifndef CORE_FILESYSTEMDEFAULTSHADERS_H
#define CORE_FILESYSTEMDEFAULTSHADERS_H


#include "core/vfs.h"

namespace euphoria::core
{
    namespace vfs
    {
        class FileSystemDefaultShaders : public FileSystemReadRoot
        {
        public:
            static void
            AddRoot(FileSystem* fs, const std::string& base);

            std::shared_ptr<MemoryChunk>
            ReadFile(const std::string& path) override;

            std::string
            Describe() override;

            explicit FileSystemDefaultShaders(const std::string& base);

            FileList
            ListFiles(const Path& path) override;

        private:
            std::string base_;
        };

    }  // namespace vfs

}  // namespace euphoria::core

#endif  // CORE_FILESYSTEMDEFAULTSHADERS_H
