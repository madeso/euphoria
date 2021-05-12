#pragma once


#include "core/memorychunk.h"

#include <string>
#include <map>
#include <memory>
#include <vector>


namespace euphoria::core::vfs
{
    struct file_path;
    struct dir_path;

    struct listed_file
    {
        std::string name;
        bool is_builtin = false;
        bool is_file = false;

        listed_file() = default;
        listed_file(const std::string& n, bool b, bool f);
    };

    struct file_list
    {
        std::map<std::string, listed_file> files;
        std::map<std::string, listed_file> folders;

        void
        add(const listed_file& file);

        void
        add(const std::string& n, bool b, bool f);
    };

    // todo(Gustav): use path class

    struct read_root
    {
        read_root() = default;
        virtual ~read_root();

        read_root(const read_root&) = delete;
        read_root(read_root&&) = delete;
        void operator=(const read_root&) = delete;
        void operator=(read_root&&) = delete;

        virtual void
        add_description(std::vector<std::string>* strings) = 0;

        virtual std::shared_ptr<memory_chunk>
        read_file(const file_path& path) = 0;

        virtual file_list
        list_files(const dir_path& path) = 0;
    };

    struct write_root
    {
        write_root() = default;
        virtual ~write_root();

        write_root(const write_root&) = delete;
        write_root(write_root&&) = delete;
        void operator=(const write_root&) = delete;
        void operator=(write_root&&) = delete;

        virtual void
        write_file(const file_path& path, std::shared_ptr<memory_chunk> data) = 0;
    };

    struct file_system
    {
        file_system();
        ~file_system();

        file_system(const file_system&) = delete;
        file_system(file_system&&) = delete;
        void operator=(const file_system&) = delete;
        void operator=(file_system&&) = delete;

        void
        add_read_root(const std::shared_ptr<read_root>& root);

        void
        set_write_root(const std::shared_ptr<vfs::write_root>& root);

        std::shared_ptr<memory_chunk>
        read_file(const file_path& path);

        void
        write_file(const file_path& path, std::shared_ptr<memory_chunk> data);

        std::vector<listed_file>
        list_files(const dir_path& path);

        std::string
        get_roots_as_string();

        // todo(Gustav): need to support paging too
        bool
        read_file_to_string(const file_path& path, std::string* source);

        // todo(Gustav): support different roots such as real file system, zip/container file
        // etc
        // todo(Gustav): support encryption
        // todo(Gustav): support listing/enumerating files


        std::vector<std::shared_ptr<read_root>> read_roots;
        std::shared_ptr<vfs::write_root> write_root;
    };

    struct read_root_catalog : read_root
    {
        read_root_catalog();

        // todo(Gustav): allow registering string_view and embedded binary chunks

        void
        register_file_string
        (
            const file_path& path,
            const std::string& content
        );

        void
        register_file_data
        (
            const file_path& path,
            const std::shared_ptr<memory_chunk>& content
        );

        static std::shared_ptr<read_root_catalog>
        create_and_add(file_system* fs);

        std::shared_ptr<memory_chunk>
        read_file(const file_path& path) override;

        void
        add_description(std::vector<std::string>* strings) override;

        file_list
        list_files(const dir_path& path) override;

    private:
        std::map<file_path, std::shared_ptr<memory_chunk>> catalog_;
    };

    struct read_root_physical_folder : read_root
    {
        explicit read_root_physical_folder(std::string folder);

        std::shared_ptr<memory_chunk>
        read_file(const file_path& path) override;

        static void
        add(file_system* fs, const std::string& folder);

        static void
        add_current_directory(file_system* fs);

        void
        add_description(std::vector<std::string>* strings) override;

        file_list
        list_files(const dir_path& path) override;
        
        std::string folder;
    };

    struct write_root_physical_folder : write_root
    {
        explicit write_root_physical_folder(const std::string& f);

        void
        write_file
        (
            const file_path& path,
            std::shared_ptr<memory_chunk> data
        ) override;

        std::string folder;
    };
}

