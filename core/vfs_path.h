#ifndef EUPHORIA_PATH_H
#define EUPHORIA_PATH_H

#include <string>

namespace euphoria::core
{
    namespace vfs
    {
        // represents a virtual path
        // always lowercase
        // forward slash specify a directory
        // todo: transform into a path and directory class instead
        struct Path
        {
        public:
            [[nodiscard]]
            static Path
            FromDirectory(const std::string& path);

            [[nodiscard]]
            static Path
            FromRoot();

            [[nodiscard]]
            static Path
            FromFile(const std::string& path);

            [[nodiscard]]
            static Path
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
