#include "core/vfs.h"

#include <fstream>

#include "core/assert.h"
#include "core/os.h"
#include "core/stringmerger.h"
#include "core/vfs_path.h"
#include "core/io.h"
#include "core/log.h"

#include "core/stringutils.h"


namespace euphoria::core::vfs
{
    ////////////////////////////////////////////////////////////////////////////////

    ListedFile::ListedFile(const std::string& n, bool b, bool f)
        : name(n), is_builtin(b), is_file(f)
    {}

    ////////////////////////////////////////////////////////////////////////////////

    void
    FileList::add(const ListedFile& file)
    {
        auto& target = file.is_file ? files : folders;
        const auto found = target.find(file.name);
        if(found == target.end())
        {
            target[file.name] = file;
        }
    }

    void
    FileList::add(const std::string& n, bool b, bool f)
    {
        add(ListedFile {n, b, f});
    }

    ////////////////////////////////////////////////////////////////////////////////

    ReadRoot::~ReadRoot() = default;

    WriteRoot::~WriteRoot() = default;

    ////////////////////////////////////////////////////////////////////////////////

    FileSystem::FileSystem() = default;

    FileSystem::~FileSystem() = default;

    void
    FileSystem::add_read_root(const std::shared_ptr<ReadRoot>& root)
    {
        read_roots.push_back(root);
    }

    void
    FileSystem::set_write_root(const std::shared_ptr<vfs::WriteRoot>& root)
    {
        write_root = root;
    }

    std::shared_ptr<MemoryChunk>
    FileSystem::read_file(const FilePath& a_path)
    {
        FilePath path = a_path;

        if(a_path.get_directory().contains_relative())
        {
            const auto resolved_path = resolve_relative(a_path, DirPath::from_root());
            if(resolved_path.has_value() == false)
            {
                LOG_ERROR("Unable to resolve path to a valid path {0}", a_path);
                return MemoryChunk::null();
            }
            path = resolved_path.value();
        }

        for(auto& root: read_roots)
        {
            std::shared_ptr<MemoryChunk> file = root->read_file(path);
            if(file != nullptr)
            {
                return file;
            }
        }

        return MemoryChunk::null();
    }

    void
    FileSystem::write_file
    (
        const FilePath& path,
        std::shared_ptr<MemoryChunk> data
    )
    {
        ASSERT(write_root);
        write_root->write_file(path, data);
    }

    std::vector<ListedFile>
    FileSystem::list_files(const DirPath& path)
    {
        FileList combined;
        for(auto& root: read_roots)
        {
            const auto files = root->list_files(path);
            for(const auto& f: files.files)
            {
                combined.add(f.second);
            }
            for(const auto& f: files.folders)
            {
                combined.add(f.second);
            }
        }

        std::vector<ListedFile> r;
        for(const auto& f: combined.folders)
        {
            r.emplace_back(f.second);
        }
        for(const auto& f: combined.files)
        {
            r.emplace_back(f.second);
        }
        return r;
    }

    std::string
    FileSystem::get_roots_as_string()
    {
        std::vector<std::string> ret;
        for(auto& root: read_roots)
        {
            root->add_description(&ret);
        }

        return string_mergers::array.merge(ret);
    }

    bool
    FileSystem::read_file_to_string
    (
            const FilePath& path,
            std::string* source
    )
    {
        ASSERT(source);

        std::shared_ptr<MemoryChunk> file = read_file(path);
        if(file == nullptr)
        {
            return false;
        }

        // data is unsigned char, cast it to char and assume it is a string
        *source = std::string(file->get_data(), file->get_size() - 1);
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////

    ReadRootCatalog::ReadRootCatalog() = default;

    void
    ReadRootCatalog::register_file_string
    (
        const FilePath& path,
        const std::string& content
    )
    {
        std::shared_ptr<MemoryChunk> file = create_memory_chunk_from_string(content);
        register_file_data(path, file);
    }

    void
    ReadRootCatalog::register_file_data
    (
        const FilePath& path,
        const std::shared_ptr<MemoryChunk>& content
    )
    {
        catalog.insert(std::make_pair(path, content));
    }

    std::shared_ptr<ReadRootCatalog>
    ReadRootCatalog::create_and_add(FileSystem* fs)
    {
        ASSERT(fs);
        auto catalog = std::make_shared<ReadRootCatalog>();
        fs->add_read_root(catalog);
        return catalog;
    }

    std::shared_ptr<MemoryChunk>
    ReadRootCatalog::read_file(const FilePath& path)
    {
        const auto found = catalog.find(path);

        if(found == catalog.end())
        {
            return MemoryChunk::null();
        }

        return found->second;
    }

    void
    ReadRootCatalog::add_description(std::vector<std::string>* strings)
    {
        const auto keys = key_to_string_vector(catalog);
        strings->insert(strings->end(), keys.begin(), keys.end());
    }

    FileList
    ReadRootCatalog::list_files(const DirPath& path)
    {
        FileList r;
        for(const auto& f: catalog)
        {
            const auto file = f.first;
            const auto [folder, filename] = file.split_directories_and_file();
            if(path == folder)
            {
                r.add(filename, true, true);
            }
        }

        return r;
    }

    ////////////////////////////////////////////////////////////////////////////////

    std::string
    combine_folder_and_path(const std::string& folder, const FilePath& path)
    {
        return folder + path.path.substr(2);
    }

    std::string
    combine_folder_and_path(const std::string& folder, const DirPath& path)
    {
        return folder + path.path.substr(2);
    }

    std::string
    make_sure_folder_ends_with_slash(const std::string& folder)
    {
        const std::string slash = "/";

        const std::string the_folder
                = ends_with(folder, slash) ? folder : folder + slash;
        return the_folder;
    }

    ////////////////////////////////////////////////////////////////////////////////

    ReadRootPhysicalFolder::ReadRootPhysicalFolder(std::string folder)
        : folder(std::move(folder))
    {}

    std::shared_ptr<MemoryChunk>
    ReadRootPhysicalFolder::read_file(const FilePath& path)
    {
        const std::string& full_path = combine_folder_and_path(folder, path);
        return io::file_to_chunk(full_path);
    }

    void
    ReadRootPhysicalFolder::add_description(std::vector<std::string>* strings)
    {
        strings->emplace_back(folder);
    }

    void
    ReadRootPhysicalFolder::add(FileSystem* fs, const std::string& folder)
    {
        ASSERT(fs);

        auto catalog = std::make_shared<ReadRootPhysicalFolder>(make_sure_folder_ends_with_slash(folder));

        fs->add_read_root(catalog);
    }

    void
    ReadRootPhysicalFolder::add_current_directory(FileSystem* fs)
    {
        const std::string folder = get_current_directory();
        return ReadRootPhysicalFolder::add(fs, folder);
    }

    FileList
    ReadRootPhysicalFolder::list_files(const DirPath& path)
    {
        const auto real_path
                = combine_folder_and_path(folder, path);
        const auto found = list_directory(real_path);

        FileList r;

        if(found.valid)
        {
            for(const auto& f: found.files)
            {
                r.add(f, false, true);
            }
            for(const auto& d: found.directories)
            {
                auto f = d;
                if(!ends_with(f, '/'))
                {
                    f += +"/";
                }
                r.add(f, false, false);
            }
        }

        return r;
    }

    ////////////////////////////////////////////////////////////////////////////////

    WriteRootPhysicalFolder::WriteRootPhysicalFolder(const std::string& f)
        : folder(make_sure_folder_ends_with_slash(f))
    {}

    void
    WriteRootPhysicalFolder::write_file
    (
        const FilePath& path,
        std::shared_ptr<MemoryChunk> data
    )
    {
        const auto full_path = combine_folder_and_path(folder, path);
        io::chunk_to_file(data, full_path);
    }

}
