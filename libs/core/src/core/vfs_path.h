#ifndef EUPHORIA_PATH_H
#define EUPHORIA_PATH_H

#include <string>
#include <vector>
#include <optional>
#include <tuple>
#include <type_traits>

namespace euphoria::core
{
    struct Str;

    namespace vfs
    {
        // here lies the definitions of a virtual path
        // can either be a file or a directory
        // (prefer directory to folder, since it can autocomplete from 1 char)
        //
        // a path is case sensitive
        // a path should be lowercase, but this is not required
        // a path should only contain ascii characters but this is not required
        // a slash (/) seperates folders or directories
        //
        // File spceifications
        // filename or extension cannot contain slashes (/)
        // filenames cannot contain dots (.)
        // extension is either empty, or a string like txt or tar.gz and
        //   can't start/end with a dot (.)
        //
        // Directory specifications
        // tile (~) respresents the root folder (absolute path)
        // a single dot (.) represesents the "current" folder (relative path)
        // twot dots (..) represents the parent folder (relative path)

        struct FilePath;
        struct DirPath;


        struct FilePath
        {
            // apply only minor changes, return null on invalid
            static
            std::optional<FilePath>
            FromScript(const std::string& path);


            // do everything possible to convert from dirty path to valid path
            static
            std::optional<FilePath>
            FromDirtySource(const std::string& path);


            // optional or not, log if error
            static
            std::optional<FilePath>
            FromScriptOrEmpty(const std::string& path);


            [[nodiscard]] std::tuple<DirPath, std::string>
            SplitDirectoriesAndFile() const;


            [[nodiscard]] DirPath
            GetDirectory() const;


            [[nodiscard]] std::string
            GetFileWithExtension() const;


            [[nodiscard]] std::string
            GetFilenameWithoutExtension() const;


            [[nodiscard]] std::string
            GetExtension() const;


            [[nodiscard]] FilePath
            SetExtensionCopy(const std::string& ext) const;


            [[nodiscard]] FilePath
            ExtendExtensionCopy(const std::string& ext) const;


            explicit
            FilePath(const std::string& p);


            template
            <
                typename OStream,
                typename X = std::enable_if_t
                <
                    !std::is_same<OStream, core::Str>::value
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
            FromRoot();


            [[nodiscard]]
            static DirPath
            FromRelative();


            [[nodiscard]]
            static DirPath
            FromDirs(const std::vector<std::string>& dirs);


            [[nodiscard]] FilePath
            GetFile(const std::string& filename) const;


            [[nodiscard]] bool
            IsRelative() const;


            [[nodiscard]] bool
            ContainsRelative() const;


            [[nodiscard]] DirPath
            GetParentDirectory() const;


            [[nodiscard]] std::string
            GetDirectoryName() const;


            [[nodiscard]]
            std::vector<std::string>
            SplitDirectories() const;


            [[nodiscard]]
            DirPath
            SingleCdCopy(const std::string& single) const;


            explicit
            DirPath(const std::string& p);


            template
            <
                typename OStream,
                typename X = std::enable_if_t
                <
                    !std::is_same<OStream, core::Str>::value
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
        ResolveRelative(const DirPath& base);


        std::optional<DirPath>
        ResolveRelative(const DirPath& base, const DirPath& root);


        std::optional<FilePath>
        ResolveRelative(const FilePath& base);


        std::optional<FilePath>
        ResolveRelative(const FilePath& base, const DirPath& root);


        DirPath
        Join(const DirPath& lhs, const DirPath& rhs);


        FilePath
        Join(const DirPath& lhs, const FilePath& rhs);


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
    }  // namespace vfs

}  // namespace euphoria::core

#endif  // EUPHORIA_PATH_H
