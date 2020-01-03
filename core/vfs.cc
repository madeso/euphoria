#include "core/vfs.h"

#include <fstream>

#include "core/assert.h"
#include "core/os.h"
#include "core/stringmerger.h"
#include "core/vfs_path.h"
#include "core/io.h"

#include "core/stringutils.h"

namespace euphoria::core
{
    namespace vfs
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

        std::shared_ptr<MemoryChunk>
        FileSystem::ReadFile(const FilePath& path)
        {
            for(auto& root: roots_)
            {
                std::shared_ptr<MemoryChunk> file = root->ReadFile(path);
                if(file != nullptr)
                {
                    return file;
                }
            }

            return MemoryChunk::Null();
        }

        void
        FileSystem::WriteFile
        (
            const FilePath& path,
            std::shared_ptr<MemoryChunk> data
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

            std::shared_ptr<MemoryChunk> file = ReadFile(path);
            if(file == nullptr)
            {
                return false;
            }

            // data is unsigned char, cast it to char and assume it is a string
            *source = std::string(file->GetData(), file->GetSize() - 1);
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
            std::shared_ptr<MemoryChunk> file = MemoryChunkFromText(content);
            RegisterFileData(path, file);
        }

        void
        FileSystemRootCatalog::RegisterFileData
        (
            const FilePath& path,
            const std::shared_ptr<MemoryChunk>& content
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

        std::shared_ptr<MemoryChunk>
        FileSystemRootCatalog::ReadFile(const FilePath& path)
        {
            const auto found = catalog_.find(path);

            if(found == catalog_.end())
            {
                return MemoryChunk::Null();
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

        std::shared_ptr<MemoryChunk>
        FileSystemRootFolder::ReadFile(const FilePath& path)
        {
            const std::string& full_path = CombineFolderAndPath(folder_, path);
            return io::FileToChunk(full_path);
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
            const std::string folder = GetCurrentDirectory();
            return FileSystemRootFolder::AddRoot(fs, folder);
        }

        FileList
        FileSystemRootFolder::ListFiles(const DirPath& path)
        {
            const auto real_path
                    = CombineFolderAndPath(folder_, path);
            const auto found = ListDirectory(real_path);

            FileList r;

            if(found.valid)
            {
                for(const auto& f: found.files)
                {
                    r.Add(f, false, false);
                }
                for(const auto& d: found.directories)
                {
                    auto f = d;
                    if(!EndsWith(f, '/'))
                    {
                        f += +"/";
                    }
                    r.Add(f, false, true);
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
            std::shared_ptr<MemoryChunk> data
        )
        {
            const auto full_path = CombineFolderAndPath(folder, path);
            io::ChunkToFile(data, full_path);
        }

    }  // namespace vfs

}  // namespace euphoria::core
