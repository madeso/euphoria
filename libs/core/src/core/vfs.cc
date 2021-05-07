#include "core/vfs.h"

#include <fstream>

#include "core/assert.h"
#include "core/os.h"
#include "core/stringmerger.h"
#include "core/vfs_path.h"
#include "core/io.h"
#include "core/log.h"

#include "core/stringutils.h"


LOG_SPECIFY_DEFAULT_LOGGER("core.vfs")


namespace euphoria::core::vfs
{
    ////////////////////////////////////////////////////////////////////////////////

    ListedFile::ListedFile(const std::string& n, bool b, bool f)
        : name(n), is_builtin(b), is_file(f)
    {}

    ////////////////////////////////////////////////////////////////////////////////

    void
    FileList::Add(const ListedFile& file)
    {
        auto&      target = file.is_file ? files : folders;
        const auto found  = target.find(file.name);
        if(found == target.end())
        {
            target[file.name] = file;
        }
    }

    void
    FileList::Add(const std::string& n, bool b, bool f)
    {
        Add(ListedFile {n, b, f});
    }

    ////////////////////////////////////////////////////////////////////////////////

    FileSystemReadRoot::~FileSystemReadRoot() = default;

    FileSystemWriteRoot::~FileSystemWriteRoot() = default;

    ////////////////////////////////////////////////////////////////////////////////

    FileSystem::FileSystem() = default;

    FileSystem::~FileSystem() = default;

    void
    FileSystem::AddReadRoot(const std::shared_ptr<FileSystemReadRoot>& root)
    {
        roots_.push_back(root);
    }

    void
    FileSystem::SetWrite(const std::shared_ptr<FileSystemWriteRoot>& root)
    {
        write_ = root;
    }

    std::shared_ptr<memory_chunk>
    FileSystem::ReadFile(const FilePath& a_path)
    {
        FilePath path = a_path;

        if(a_path.GetDirectory().ContainsRelative())
        {
            const auto resolved_path = ResolveRelative(a_path, DirPath::FromRoot());
            if(resolved_path.has_value() == false)
            {
                LOG_ERROR("Unable to resolve path to a valid path {0}", a_path);
                return memory_chunk::null();
            }
            path = resolved_path.value();
        }

        for(auto& root: roots_)
        {
            std::shared_ptr<memory_chunk> file = root->ReadFile(path);
            if(file != nullptr)
            {
                return file;
            }
        }

        return memory_chunk::null();
    }

    void
    FileSystem::WriteFile
    (
        const FilePath& path,
        std::shared_ptr<memory_chunk> data
    )
    {
        ASSERT(write_);
        write_->WriteFile(path, data);
    }

    std::vector<ListedFile>
    FileSystem::ListFiles(const DirPath& path)
    {
        FileList combined;
        for(auto& root: roots_)
        {
            const auto files = root->ListFiles(path);
            for(const auto& f: files.files)
            {
                combined.Add(f.second);
            }
            for(const auto& f: files.folders)
            {
                combined.Add(f.second);
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
    FileSystem::GetRootsAsString()
    {
        std::vector<std::string> ret;
        for(auto& root: roots_)
        {
            root->Describe(&ret);
        }

        return StringMerger::Array().Generate(ret);
    }

    bool
    FileSystem::ReadFileToString
    (
            const FilePath& path,
            std::string* source
    )
    {
        ASSERT(source);

        std::shared_ptr<memory_chunk> file = ReadFile(path);
        if(file == nullptr)
        {
            return false;
        }

        // data is unsigned char, cast it to char and assume it is a string
        *source = std::string(file->get_data(), file->get_size() - 1);
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////

    FileSystemRootCatalog::FileSystemRootCatalog() = default;

    void
    FileSystemRootCatalog::RegisterFileString
    (
        const FilePath& path,
        const std::string& content
    )
    {
        std::shared_ptr<memory_chunk> file = create_memory_chunk_from_string(content);
        RegisterFileData(path, file);
    }

    void
    FileSystemRootCatalog::RegisterFileData
    (
        const FilePath& path,
        const std::shared_ptr<memory_chunk>& content
    )
    {
        catalog_.insert(std::make_pair(path, content));
    }

    std::shared_ptr<FileSystemRootCatalog>
    FileSystemRootCatalog::AddRoot(FileSystem* fs)
    {
        ASSERT(fs);
        auto catalog = std::make_shared<FileSystemRootCatalog>();
        fs->AddReadRoot(catalog);
        return catalog;
    }

    std::shared_ptr<memory_chunk>
    FileSystemRootCatalog::ReadFile(const FilePath& path)
    {
        const auto found = catalog_.find(path);

        if(found == catalog_.end())
        {
            return memory_chunk::null();
        }

        return found->second;
    }

    void
    FileSystemRootCatalog::Describe(std::vector<std::string>* strings)
    {
        const auto keys = KeyToStringVector(catalog_);
        strings->insert(strings->end(), keys.begin(), keys.end());
    }

    FileList
    FileSystemRootCatalog::ListFiles(const DirPath& path)
    {
        FileList r;
        for(const auto& f: catalog_)
        {
            const auto file   = f.first;
            const auto [folder, filename] = file.SplitDirectoriesAndFile();
            if(path == folder)
            {
                r.Add(filename, true, true);
            }
        }

        return r;
    }

    ////////////////////////////////////////////////////////////////////////////////

    std::string
    CombineFolderAndPath(const std::string& folder, const FilePath& path)
    {
        return folder + path.path.substr(2);
    }

    std::string
    CombineFolderAndPath(const std::string& folder, const DirPath& path)
    {
        return folder + path.path.substr(2);
    }

    std::string
    MakeSureEndsWithSlash(const std::string& folder)
    {
        const std::string slash = "/";

        const std::string the_folder
                = EndsWith(folder, slash) ? folder : folder + slash;
        return the_folder;
    }

    ////////////////////////////////////////////////////////////////////////////////

    FileSystemRootFolder::FileSystemRootFolder(std::string folder)
        : folder_(std::move(folder))
    {}

    std::shared_ptr<memory_chunk>
    FileSystemRootFolder::ReadFile(const FilePath& path)
    {
        const std::string& full_path = CombineFolderAndPath(folder_, path);
        return io::file_to_chunk(full_path);
    }

    void
    FileSystemRootFolder::Describe(std::vector<std::string>* strings)
    {
        strings->emplace_back(folder_);
    }

    void
    FileSystemRootFolder::AddRoot(FileSystem* fs, const std::string& folder)
    {
        ASSERT(fs);

        auto catalog = std::make_shared<FileSystemRootFolder>(
                MakeSureEndsWithSlash(folder));

        fs->AddReadRoot(catalog);
    }

    void
    FileSystemRootFolder::AddRoot(FileSystem* fs)
    {
        const std::string folder = get_current_directory();
        return FileSystemRootFolder::AddRoot(fs, folder);
    }

    FileList
    FileSystemRootFolder::ListFiles(const DirPath& path)
    {
        const auto real_path
                = CombineFolderAndPath(folder_, path);
        const auto found = list_directory(real_path);

        FileList r;

        if(found.valid)
        {
            for(const auto& f: found.files)
            {
                r.Add(f, false, true);
            }
            for(const auto& d: found.directories)
            {
                auto f = d;
                if(!ends_with(f, '/'))
                {
                    f += +"/";
                }
                r.Add(f, false, false);
            }
        }

        return r;
    }

    ////////////////////////////////////////////////////////////////////////////////

    FileSystemWriteFolder::FileSystemWriteFolder(const std::string& f)
        : folder(MakeSureEndsWithSlash(f))
    {}

    void
    FileSystemWriteFolder::WriteFile
    (
        const FilePath& path,
        std::shared_ptr<memory_chunk> data
    )
    {
        const auto full_path = CombineFolderAndPath(folder, path);
        io::chunk_to_file(data, full_path);
    }

}
