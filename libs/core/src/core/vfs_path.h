#pragma once

#include <string>
#include <vector>
#include <optional>
#include <tuple>
#include <type_traits>

namespace euphoria::core
{
    struct string_builder;

    namespace vfs
    {
        // here lies the definitions of a virtual path
        // can either be a file or a directory
        // (prefer directory to folder, since it can autocomplete from 1 char)
        //
        // a path is case sensitive
        // a path should be lowercase, but this is not required
        // a path should only contain ascii characters but this is not required
        // a slash (/) separates folders or directories
        //
        // File specifications
        // filename or extension cannot contain slashes (/)
        // filenames cannot contain dots (.)
        // extension is either empty, or a string like txt or tar.gz and
        //   can't start/end with a dot (.)
        //
        // Directory specifications
        // tilde (~) represents the root folder (absolute path)
        // a single dot (.) represents the "current" folder (relative path)
        // two dots (..) represents the parent folder (relative path)

        struct file_path;
        struct dir_path;


        struct file_path
        {
            // apply only minor changes, return null on invalid
            static
            std::optional<file_path>
            from_script(const std::string& path);


            // do everything possible to convert from dirty path to valid path
            static
            std::optional<file_path>
            from_dirty_source(const std::string& path);


            // optional or not, log if error
            static
            std::optional<file_path>
            from_script_or_empty(const std::string& path);


            [[nodiscard]] std::tuple<dir_path, std::string>
            split_directories_and_file() const;


            [[nodiscard]] dir_path
            get_directory() const;


            [[nodiscard]] std::string
            get_file_with_extension() const;


            [[nodiscard]] std::string
            get_filename_without_extension() const;


            [[nodiscard]] std::string
            get_extension() const;


            [[nodiscard]] file_path
            set_extension_copy(const std::string& ext) const;


            [[nodiscard]] file_path
            extend_extension_copy(const std::string& ext) const;


            explicit
            file_path(const std::string& p);


            template
            <
                typename OStream,
                typename X = std::enable_if_t
                <
                    !std::is_same<OStream, core::string_builder>::value
                >
            >
            friend OStream& operator<<(OStream& os, const file_path& p)
            {
                os << p.path;
                return os;
            }


            // contains either ./ or ~/ at the start
            // has a file name and possible extension
            std::string path;
        };


        struct dir_path
        {
            [[nodiscard]]
            static dir_path
            from_root();


            [[nodiscard]]
            static dir_path
            from_relative();


            [[nodiscard]]
            static dir_path
            from_dirs(const std::vector<std::string>& dirs);


            [[nodiscard]] file_path
            get_file(const std::string& filename) const;


            [[nodiscard]] bool
            is_relative() const;


            [[nodiscard]] bool
            contains_relative() const;


            [[nodiscard]] dir_path
            get_parent_directory() const;


            [[nodiscard]] std::string
            get_directory_name() const;


            [[nodiscard]]
            std::vector<std::string>
            split_directories() const;


            [[nodiscard]]
            dir_path
            single_cd_copy(const std::string& single) const;


            explicit
            dir_path(const std::string& p);


            template
            <
                typename OStream,
                typename X = std::enable_if_t
                <
                    !std::is_same<OStream, core::string_builder>::value
                >
            >
            friend OStream& operator<<(OStream& os, const dir_path& p)
            {
                os << p.path;
                return os;
            }


            // contains either . or ~ at the start, / at the end
            std::string path;
        };


        std::optional<dir_path>
        resolve_relative(const dir_path& base);


        std::optional<dir_path>
        resolve_relative(const dir_path& base, const dir_path& root);


        std::optional<file_path>
        resolve_relative(const file_path& base);


        std::optional<file_path>
        resolve_relative(const file_path& base, const dir_path& root);


        dir_path
        join(const dir_path& lhs, const dir_path& rhs);


        file_path
        join(const dir_path& lhs, const file_path& rhs);


        bool
        operator==(const dir_path& lhs, const dir_path& rhs);


        bool
        operator==(const file_path& lhs, const file_path& rhs);


        bool
        operator!=(const dir_path& lhs, const dir_path& rhs);


        bool
        operator!=(const file_path& lhs, const file_path& rhs);


        bool
        operator<(const dir_path& lhs, const dir_path& rhs);


        bool
        operator<(const file_path& lhs, const file_path& rhs);
    } // namespace vfs

}

