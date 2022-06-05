#pragma once



#include <optional>
#include <tuple>
#include <type_traits>

namespace euphoria::core
{
    struct StringBuilder;

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

        struct FilePath;
        struct DirPath;


        struct FilePath
        {
            // apply only minor changes, return null on invalid
            static
            std::optional<FilePath>
            from_script(const std::string& path);


            // do everything possible to convert from dirty path to valid path
            static
            std::optional<FilePath>
            from_dirty_source(const std::string& path);


            // optional or not, log if error
            static
            std::optional<FilePath>
            from_script_or_empty(const std::string& path);


            [[nodiscard]] std::tuple<DirPath, std::string>
            split_directories_and_file() const;


            [[nodiscard]] DirPath
            get_directory() const;


            [[nodiscard]] std::string
            get_file_with_extension() const;


            [[nodiscard]] std::string
            get_filename_without_extension() const;


            [[nodiscard]] std::string
            get_extension() const;


            [[nodiscard]] FilePath
            set_extension_copy(const std::string& ext) const;


            [[nodiscard]] FilePath
            extend_extension_copy(const std::string& ext) const;


            explicit
            FilePath(const std::string& p);


            template
            <
                typename OStream,
                typename X = std::enable_if_t
                <
                    !std::is_same<OStream, core::StringBuilder>::value
                >
            >
            friend OStream& operator<<(OStream& os, const FilePath& p)
            {
                os << p.path;
                return os;
            }


            // contains either ./ or ~/ at the start
            // has a file name and possible extension
            std::string path;
        };


        struct DirPath
        {
            [[nodiscard]]
            static DirPath
            from_root();


            [[nodiscard]]
            static DirPath
            from_relative();


            [[nodiscard]]
            static DirPath
            from_dirs(const std::vector<std::string>& dirs);


            [[nodiscard]] FilePath
            get_file(const std::string& filename) const;


            [[nodiscard]] bool
            is_relative() const;


            [[nodiscard]] bool
            contains_relative() const;


            [[nodiscard]] DirPath
            get_parent_directory() const;


            [[nodiscard]] std::string
            get_directory_name() const;


            [[nodiscard]]
            std::vector<std::string>
            split_directories() const;


            [[nodiscard]]
            DirPath
            single_cd_copy(const std::string& single) const;


            explicit
            DirPath(const std::string& p);


            template
            <
                typename OStream,
                typename X = std::enable_if_t
                <
                    !std::is_same<OStream, core::StringBuilder>::value
                >
            >
            friend OStream& operator<<(OStream& os, const DirPath& p)
            {
                os << p.path;
                return os;
            }


            // contains either . or ~ at the start, / at the end
            std::string path;
        };


        std::optional<DirPath>
        resolve_relative(const DirPath& base);


        std::optional<DirPath>
        resolve_relative(const DirPath& base, const DirPath& root);


        std::optional<FilePath>
        resolve_relative(const FilePath& base);


        std::optional<FilePath>
        resolve_relative(const FilePath& base, const DirPath& root);


        DirPath
        join(const DirPath& lhs, const DirPath& rhs);


        FilePath
        join(const DirPath& lhs, const FilePath& rhs);


        bool
        operator==(const DirPath& lhs, const DirPath& rhs);


        bool
        operator==(const FilePath& lhs, const FilePath& rhs);


        bool
        operator!=(const DirPath& lhs, const DirPath& rhs);


        bool
        operator!=(const FilePath& lhs, const FilePath& rhs);


        bool
        operator<(const DirPath& lhs, const DirPath& rhs);


        bool
        operator<(const FilePath& lhs, const FilePath& rhs);
    }

}

