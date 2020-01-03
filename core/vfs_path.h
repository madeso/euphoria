#ifndef EUPHORIA_PATH_H
#define EUPHORIA_PATH_H

#include <string>
#include <vector>
#include <optional>
#include <tuple>

namespace euphoria::core
{
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

        struct PathToFile;
        struct PathToDirectory;


        struct PathToFile
        {
            std::tuple<PathToDirectory, std::string>
            SplitDirectoriesAndFile() const;


            PathToDirectory
            GetDirectory() const;


            std::string
            GetFileWithExtension() const;


            std::string
            GetFilenameWithoutExtension() const;


            std::string
            GetExtension() const;


            explicit
            PathToFile(const std::string& p);


            // contains either ./ or ~/ at the start
            // has a file name and possible extension
            std::string path;
        };


        struct PathToDirectory
        {
            [[nodiscard]]
            static PathToDirectory
            FromRoot();


            [[nodiscard]]
            static PathToDirectory
            FromRelative();


            [[nodiscard]]
            static PathToDirectory
            FromDirs(const std::vector<std::string>& dirs);


            PathToFile
            GetFile(const std::string& filename) const;


            bool
            IsRelative() const;


            bool
            ContainsRelative() const;


            [[nodiscard]]
            std::vector<std::string>
            SplitDirectories() const;


            [[nodiscard]]
            PathToDirectory
            SingleCdCopy(const std::string& single) const;


            explicit
            PathToDirectory(const std::string& p);


            // contains either . or ~ at the start, / at the end
            std::string path;
        };


        std::optional<PathToDirectory>
        ResolveRelative
        (
            const PathToDirectory& base
        );


        std::optional<PathToDirectory>
        ResolveRelative
        (
            const PathToDirectory& base,
            const PathToDirectory& root
        );


        std::optional<PathToFile>
        ResolveRelative
        (
            const PathToFile& base
        );


        std::optional<PathToFile>
        ResolveRelative
        (
            const PathToFile& base,
            const PathToDirectory& root
        );


        PathToDirectory
        Join(const PathToDirectory& lhs, const PathToDirectory& rhs);


        PathToFile
        Join(const PathToDirectory& lhs, const PathToFile& rhs);


        /** Represents a virtual path.
         * It is always lowercase and forward slash specify a directory.
         */
        struct Path
        {
            // todo(Gustav): transform into a path and directory class instead

            [[nodiscard]] static Path
            FromDirectory(const std::string& path);

            [[nodiscard]] static Path
            FromRoot();

            [[nodiscard]] static Path
            FromFile(const std::string& path);

            [[nodiscard]] static Path
            FromGuess(const std::string& path);

            // get the directory containing the file, or this
            const Path
            GetDirectory() const;

            // only valid for directory
            const Path
            GetParentDirectory() const;

            const Path
            GetSubDirectory(const std::string& name) const;
            const Path
            GetFile(const std::string& name) const;

            bool
            IsDirectory() const;
            bool
            IsFile() const;

            const std::string&
            GetAbsolutePath() const;

            std::string
            GetDirectoryName() const;

            std::string
            GetFileName() const;

            bool
            operator==(const Path& rhs) const;

        private:
            Path(const std::string& absolute_path);
            std::string absolute_path_;
        };

    }  // namespace vfs

}  // namespace euphoria::core

#endif  // EUPHORIA_PATH_H
