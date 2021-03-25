#ifndef EUPHORIA_FILESYSTEM_H
#define EUPHORIA_FILESYSTEM_H

#include "core/memorychunk.h"

#include <string>
#include <map>
#include <memory>
#include <vector>

namespace euphoria::core::vfs
{
    struct FilePath;
    struct DirPath;

    struct ListedFile
    {
        std::string name;
        bool is_builtin = false;
        bool is_file = false;

        ListedFile() = default;
        ListedFile(const std::string& n, bool b, bool f);
    };

    struct FileList
    {
        std::map<std::string, ListedFile> files;
        std::map<std::string, ListedFile> folders;

        void
        Add(const ListedFile& file);

        void
        Add(const std::string& n, bool b, bool f);
    };

    // todo: use path class

    struct FileSystemReadRoot
    {
        FileSystemReadRoot() = default;
        virtual ~FileSystemReadRoot();

        FileSystemReadRoot(const FileSystemReadRoot&) = delete;
        FileSystemReadRoot(FileSystemReadRoot&&) = delete;
        void operator=(const FileSystemReadRoot&) = delete;
        void operator=(FileSystemReadRoot&&) = delete;

        virtual void
        Describe(std::vector<std::string>* strings)
                = 0;

        virtual std::shared_ptr<MemoryChunk>
        ReadFile(const FilePath& path) = 0;

        virtual FileList
        ListFiles(const DirPath& path)
                = 0;
    };

    struct FileSystemWriteRoot
    {
        FileSystemWriteRoot() = default;
        virtual ~FileSystemWriteRoot();

        FileSystemWriteRoot(const FileSystemWriteRoot&) = delete;
        FileSystemWriteRoot(FileSystemWriteRoot&&) = delete;
        void operator=(const FileSystemWriteRoot&) = delete;
        void operator=(FileSystemWriteRoot&&) = delete;

        virtual void
        WriteFile(const FilePath& path, std::shared_ptr<MemoryChunk> data) = 0;
    };

    struct FileSystem
    {
        FileSystem();
        ~FileSystem();

        FileSystem(const FileSystem&) = delete;
        FileSystem(FileSystem&&) = delete;
        void operator=(const FileSystem&) = delete;
        void operator=(FileSystem&&) = delete;

        void
        AddReadRoot(const std::shared_ptr<FileSystemReadRoot>& root);

        void
        SetWrite(const std::shared_ptr<FileSystemWriteRoot>& root);

        std::shared_ptr<MemoryChunk>
        ReadFile(const FilePath& path);

        void
        WriteFile(const FilePath& path, std::shared_ptr<MemoryChunk> data);

        std::vector<ListedFile>
        ListFiles(const DirPath& path);

        std::string
        GetRootsAsString();

        // todo: need to support paging too
        bool
        ReadFileToString(const FilePath& path, std::string* source);

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
                const FilePath& path,
                const std::string& content);
        void
        RegisterFileData(
                const FilePath&                  path,
                const std::shared_ptr<MemoryChunk>& content);

        static std::shared_ptr<FileSystemRootCatalog>
        AddRoot(FileSystem* fs);

        std::shared_ptr<MemoryChunk>
        ReadFile(const FilePath& path) override;

        void
        Describe(std::vector<std::string>* strings) override;

        FileList
        ListFiles(const DirPath& path) override;

    private:
        std::map<FilePath, std::shared_ptr<MemoryChunk>> catalog_;
    };

    struct FileSystemRootFolder : public FileSystemReadRoot
    {
        explicit FileSystemRootFolder(std::string folder);

        std::shared_ptr<MemoryChunk>
        ReadFile(const FilePath& path) override;

        static void
        AddRoot(FileSystem* fs, const std::string& folder);

        static void
        AddRoot(FileSystem* fs);

        void
        Describe(std::vector<std::string>* strings) override;

        FileList
        ListFiles(const DirPath& path) override;

    private:
        std::string folder_;
    };

    struct FileSystemWriteFolder : public FileSystemWriteRoot
    {
        explicit FileSystemWriteFolder(const std::string& f);

        void
        WriteFile
        (
            const FilePath& path,
            std::shared_ptr<MemoryChunk> data
        ) override;

        std::string folder;
    };
}

#endif  // EUPHORIA_FILESYSTEM_H
