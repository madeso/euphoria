#ifndef EUPHORIA_FILESYSTEM_H
#define EUPHORIA_FILESYSTEM_H

#include "core/memorychunk.h"

#include <string>
#include <map>
#include <memory>
#include <vector>

namespace euphoria::core
{
    namespace vfs
    {
        struct Path;

        struct ListedFile
        {
            std::string name;
            bool        is_builtin = false;

            ListedFile() = default;
            ListedFile(const std::string& n, bool b);
        };

        struct FileList
        {
            std::map<std::string, ListedFile> files;
            std::map<std::string, ListedFile> folders;

            void
            Add(const ListedFile& file);

            void
            Add(const std::string& n, bool b);
        };

        // todo: use path class

        struct FileSystemReadRoot
        {
            virtual ~FileSystemReadRoot();

            virtual void
            Describe(std::vector<std::string>* strings)
                    = 0;

            virtual std::shared_ptr<MemoryChunk>
            ReadFile(const std::string& path) = 0;

            virtual FileList
            ListFiles(const Path& path)
                    = 0;
        };

        struct FileSystemWriteRoot
        {
            virtual ~FileSystemWriteRoot();

            virtual void
            WriteFile(
                    const std::string&           path,
                    std::shared_ptr<MemoryChunk> data)
                    = 0;
        };

        struct FileSystem
        {
            FileSystem();
            ~FileSystem();

            void
            AddReadRoot(const std::shared_ptr<FileSystemReadRoot>& root);

            void
            SetWrite(const std::shared_ptr<FileSystemWriteRoot>& root);

            std::shared_ptr<MemoryChunk>
            ReadFile(const std::string& path);

            void
            WriteFile(
                    const std::string&           path,
                    std::shared_ptr<MemoryChunk> data);

            std::vector<ListedFile>
            ListFiles(const Path& path);

            std::string
            GetRootsAsString();

            // todo: need to support paging too
            bool
            ReadFileToString(const std::string& path, std::string* source);

            // todo: support different roots such as real file system, zip/container file
            // etc
            // todo: support encryption
            // todo: support listing/enumerating files

        private:
            std::vector<std::shared_ptr<FileSystemReadRoot>> roots_;
            std::shared_ptr<FileSystemWriteRoot>             write_;
        };

        struct FileSystemRootCatalog : public FileSystemReadRoot
        {
        public:
            FileSystemRootCatalog();

            void
            RegisterFileString(
                    const std::string& path,
                    const std::string& content);
            void
            RegisterFileData(
                    const std::string&                  path,
                    const std::shared_ptr<MemoryChunk>& content);

            static std::shared_ptr<FileSystemRootCatalog>
            AddRoot(FileSystem* fs);

            std::shared_ptr<MemoryChunk>
            ReadFile(const std::string& path) override;

            void
            Describe(std::vector<std::string>* strings) override;

            FileList
            ListFiles(const Path& path) override;

        private:
            std::map<std::string, std::shared_ptr<MemoryChunk>> catalog_;
        };

        struct FileSystemRootFolder : public FileSystemReadRoot
        {
            explicit FileSystemRootFolder(std::string folder);

            std::shared_ptr<MemoryChunk>
            ReadFile(const std::string& path) override;

            static void
            AddRoot(FileSystem* fs, const std::string& folder);

            static void
            AddRoot(FileSystem* fs);

            void
            Describe(std::vector<std::string>* strings) override;

            FileList
            ListFiles(const Path& path) override;

        private:
            std::string folder_;
        };

        struct FileSystemWriteFolder : public FileSystemWriteRoot
        {
            explicit FileSystemWriteFolder(const std::string& f);

            void
            WriteFile(
                    const std::string&           path,
                    std::shared_ptr<MemoryChunk> data) override;

            std::string folder;
        };


    }  // namespace vfs

}  // namespace euphoria::core

#endif  // EUPHORIA_FILESYSTEM_H
