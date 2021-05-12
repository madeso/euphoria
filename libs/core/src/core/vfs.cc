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

    listed_file::listed_file(const std::string& n, bool b, bool f)
        : name(n), is_builtin(b), is_file(f)
    {}

    ////////////////////////////////////////////////////////////////////////////////

    void
    file_list::add(const listed_file& file)
    {
        auto&      target = file.is_file ? files : folders;
        const auto found  = target.find(file.name);
        if(found == target.end())
        {
            target[file.name] = file;
        }
    }

    void
    file_list::add(const std::string& n, bool b, bool f)
    {
        add(listed_file {n, b, f});
    }

    ////////////////////////////////////////////////////////////////////////////////

    read_root::~read_root() = default;

    write_root::~write_root() = default;

    ////////////////////////////////////////////////////////////////////////////////

    file_system::file_system() = default;

    file_system::~file_system() = default;

    void
    file_system::add_read_root(const std::shared_ptr<read_root>& root)
    {
        read_roots.push_back(root);
    }

    void
    file_system::set_write_root(const std::shared_ptr<vfs::write_root>& root)
    {
        write_root = root;
    }

    std::shared_ptr<memory_chunk>
    file_system::read_file(const file_path& a_path)
    {
        file_path path = a_path;

        if(a_path.get_directory().contains_relative())
        {
            const auto resolved_path = resolve_relative(a_path, dir_path::from_root());
            if(resolved_path.has_value() == false)
            {
                LOG_ERROR("Unable to resolve path to a valid path {0}", a_path);
                return memory_chunk::null();
            }
            path = resolved_path.value();
        }

        for(auto& root: read_roots)
        {
            std::shared_ptr<memory_chunk> file = root->read_file(path);
            if(file != nullptr)
            {
                return file;
            }
        }

        return memory_chunk::null();
    }

    void
    file_system::write_file
    (
        const file_path& path,
        std::shared_ptr<memory_chunk> data
    )
    {
        ASSERT(write_root);
        write_root->write_file(path, data);
    }

    std::vector<listed_file>
    file_system::list_files(const dir_path& path)
    {
        file_list combined;
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

        std::vector<listed_file> r;
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
    file_system::get_roots_as_string()
    {
        std::vector<std::string> ret;
        for(auto& root: read_roots)
        {
            root->add_description(&ret);
        }

        return string_mergers::array.merge(ret);
    }

    bool
    file_system::read_file_to_string
    (
            const file_path& path,
            std::string* source
    )
    {
        ASSERT(source);

        std::shared_ptr<memory_chunk> file = read_file(path);
        if(file == nullptr)
        {
            return false;
        }

        // data is unsigned char, cast it to char and assume it is a string
        *source = std::string(file->get_data(), file->get_size() - 1);
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////

    read_root_catalog::read_root_catalog() = default;

    void
    read_root_catalog::register_file_string
    (
        const file_path& path,
        const std::string& content
    )
    {
        std::shared_ptr<memory_chunk> file = create_memory_chunk_from_string(content);
        register_file_data(path, file);
    }

    void
    read_root_catalog::register_file_data
    (
        const file_path& path,
        const std::shared_ptr<memory_chunk>& content
    )
    {
        catalog_.insert(std::make_pair(path, content));
    }

    std::shared_ptr<read_root_catalog>
    read_root_catalog::create_and_add(file_system* fs)
    {
        ASSERT(fs);
        auto catalog = std::make_shared<read_root_catalog>();
        fs->add_read_root(catalog);
        return catalog;
    }

    std::shared_ptr<memory_chunk>
    read_root_catalog::read_file(const file_path& path)
    {
        const auto found = catalog_.find(path);

        if(found == catalog_.end())
        {
            return memory_chunk::null();
        }

        return found->second;
    }

    void
    read_root_catalog::add_description(std::vector<std::string>* strings)
    {
        const auto keys = key_to_string_vector(catalog_);
        strings->insert(strings->end(), keys.begin(), keys.end());
    }

    file_list
    read_root_catalog::list_files(const dir_path& path)
    {
        file_list r;
        for(const auto& f: catalog_)
        {
            const auto file   = f.first;
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
    CombineFolderAndPath(const std::string& folder, const file_path& path)
    {
        return folder + path.path.substr(2);
    }

    std::string
    CombineFolderAndPath(const std::string& folder, const dir_path& path)
    {
        return folder + path.path.substr(2);
    }

    std::string
    MakeSureEndsWithSlash(const std::string& folder)
    {
        const std::string slash = "/";

        const std::string the_folder
                = ends_with(folder, slash) ? folder : folder + slash;
        return the_folder;
    }

    ////////////////////////////////////////////////////////////////////////////////

    read_root_physical_folder::read_root_physical_folder(std::string folder)
        : folder(std::move(folder))
    {}

    std::shared_ptr<memory_chunk>
    read_root_physical_folder::read_file(const file_path& path)
    {
        const std::string& full_path = CombineFolderAndPath(folder, path);
        return io::file_to_chunk(full_path);
    }

    void
    read_root_physical_folder::add_description(std::vector<std::string>* strings)
    {
        strings->emplace_back(folder);
    }

    void
    read_root_physical_folder::add(file_system* fs, const std::string& folder)
    {
        ASSERT(fs);

        auto catalog = std::make_shared<read_root_physical_folder>(
                MakeSureEndsWithSlash(folder));

        fs->add_read_root(catalog);
    }

    void
    read_root_physical_folder::add_current_directory(file_system* fs)
    {
        const std::string folder = get_current_directory();
        return read_root_physical_folder::add(fs, folder);
    }

    file_list
    read_root_physical_folder::list_files(const dir_path& path)
    {
        const auto real_path
                = CombineFolderAndPath(folder, path);
        const auto found = list_directory(real_path);

        file_list r;

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

    write_root_physical_folder::write_root_physical_folder(const std::string& f)
        : folder(MakeSureEndsWithSlash(f))
    {}

    void
    write_root_physical_folder::write_file
    (
        const file_path& path,
        std::shared_ptr<memory_chunk> data
    )
    {
        const auto full_path = CombineFolderAndPath(folder, path);
        io::chunk_to_file(data, full_path);
    }

}
