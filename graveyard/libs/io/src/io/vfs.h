#pragma once

#include "base/memorychunk.h"

#include <map>
#include <memory>
#include <optional>


namespace eu::io
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
        add(const ListedFile& file);

        void
        add(const std::string& n, bool b, bool f);
    };

    // todo(Gustav): use path class

    struct ReadRoot
    {
        ReadRoot() = default;
        virtual ~ReadRoot();

        ReadRoot(const ReadRoot&) = delete;
        ReadRoot(ReadRoot&&) = delete;
        void operator=(const ReadRoot&) = delete;
        void operator=(ReadRoot&&) = delete;

        virtual void
        add_description(std::vector<std::string>* strings) = 0;

        virtual std::shared_ptr<MemoryChunk>
        read_file(const FilePath& path) = 0;

        virtual FileList
        list_files(const DirPath& path) = 0;
    };

    struct WriteRoot
    {
        WriteRoot() = default;
        virtual ~WriteRoot();

        WriteRoot(const WriteRoot&) = delete;
        WriteRoot(WriteRoot&&) = delete;
        void operator=(const WriteRoot&) = delete;
        void operator=(WriteRoot&&) = delete;

        virtual void
        write_file(const FilePath& path, std::shared_ptr<MemoryChunk> data) = 0;
    };

    struct FileSystem
    {
        std::vector<std::shared_ptr<ReadRoot>> read_roots;
        std::shared_ptr<WriteRoot> write_root;

        FileSystem();
        ~FileSystem();

        FileSystem(const FileSystem&) = delete;
        FileSystem(FileSystem&&) = delete;
        void operator=(const FileSystem&) = delete;
        void operator=(FileSystem&&) = delete;

        void add_read_root(const std::shared_ptr<ReadRoot>& root);
        void set_write_root(const std::shared_ptr<WriteRoot>& root);
        std::shared_ptr<MemoryChunk> read_file(const FilePath& path);
        void write_file(const FilePath& path, std::shared_ptr<MemoryChunk> data);
        std::vector<ListedFile> list_files(const DirPath& path);
        std::string get_roots_as_string();

        // todo(Gustav): need to support paging too
        std::optional<std::string> read_file_to_string(const FilePath& path);

        // todo(Gustav): support different roots such as real file system, zip/container file etc
        // todo(Gustav): support encryption
        // todo(Gustav): support listing/enumerating files
    };

    struct ReadRootCatalog : ReadRoot
    {
        ReadRootCatalog();

        // todo(Gustav): allow registering string_view and embedded binary chunks

        void
        register_file_string
        (
            const FilePath& path,
            const std::string& content
        );

        void
        register_file_data
        (
            const FilePath& path,
            const std::shared_ptr<MemoryChunk>& content
        );

        static std::shared_ptr<ReadRootCatalog>
        create_and_add(FileSystem* fs);

        std::shared_ptr<MemoryChunk>
        read_file(const FilePath& path) override;

        void
        add_description(std::vector<std::string>* strings) override;

        FileList
        list_files(const DirPath& path) override;

    private:
        std::map<FilePath, std::shared_ptr<MemoryChunk>> catalog;
    };

    struct ReadRootPhysicalFolder : ReadRoot
    {
        explicit ReadRootPhysicalFolder(std::string folder);

        std::shared_ptr<MemoryChunk>
        read_file(const FilePath& path) override;

        static void
        add(FileSystem* fs, const std::string& folder);

        static void
        add_current_directory(FileSystem* fs);

        void
        add_description(std::vector<std::string>* strings) override;

        FileList
        list_files(const DirPath& path) override;
        
        std::string folder;
    };

    struct WriteRootPhysicalFolder : WriteRoot
    {
        explicit WriteRootPhysicalFolder(const std::string& f);

        void
        write_file
        (
            const FilePath& path,
            std::shared_ptr<MemoryChunk> data
        ) override;

        std::string folder;
    };
}

