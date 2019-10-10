#include "core/vfs_path.h"

#include "core/assert.h"
#include "core/stringutils.h"

namespace euphoria::core
{
    namespace vfs
    {
        char
        DirectoryChar()
        {
            return '/';
        }

        std::string
        DirectoryString()
        {
            return "/";
        }

        Path
        Path::FromDirectory(const std::string& path)
        {
            if(path.empty() || EndsWith(path, DirectoryString()))
            {
                return Path {path};
            }
            else
            {
                return Path {path + DirectoryString()};
            }
        }

        Path
        Path::FromRoot()
        {
            return Path {""};
        }


        Path
        Path::FromFile(const std::string& path)
        {
            return Path {path};
        }

        Path
        Path::FromGuess(const std::string& path)
        {
            return Path {path};
        }

        const Path
        Path::GetDirectory() const
        {
            if(IsDirectory())
            {
                return *this;
            }
            else
            {
                const auto dir_char = absolute_path_.rfind(DirectoryChar());
                if(dir_char == std::string::npos)
                {
                    // this is a root file, so return root directory
                    return FromDirectory("");
                }
                else
                {
                    return FromDirectory(absolute_path_.substr(0, dir_char));
                }
            }
        }

        const Path
        Path::GetParentDirectory() const
        {
            if(absolute_path_.empty())
            {
                return *this;
            }
            ASSERTX(IsDirectory(), absolute_path_);
            if(!IsDirectory())
            {
                return FromDirectory("not a directory");
            }

            const auto dir_char = absolute_path_.rfind(
                    DirectoryChar(), absolute_path_.length() - 2);
            if(dir_char == std::string::npos)
            {
                // this is a root file, so return root directory
                return FromDirectory("");
            }

            return FromDirectory(absolute_path_.substr(0, dir_char));
        }

        const Path
        Path::GetSubDirectory(const std::string& name) const
        {
            ASSERT(IsDirectory());
            ASSERT(name.find(DirectoryChar()) == std::string::npos);

            return FromDirectory(absolute_path_ + name);
        }

        const Path
        Path::GetFile(const std::string& name) const
        {
            ASSERT(IsDirectory());
            ASSERT(!EndsWith(name, DirectoryString()));

            if(StartsWith(name, "~"))
            {
                return FromFile(name.substr(1));
            }

            return FromFile(absolute_path_ + name);
        }

        bool
        Path::IsDirectory() const
        {
            return absolute_path_.empty() || EndsWith(absolute_path_, "/");
        }

        bool
        Path::IsFile() const
        {
            return !absolute_path_.empty() && !EndsWith(absolute_path_, "/");
        }

        const std::string&
        Path::GetAbsolutePath() const
        {
            return absolute_path_;
        }

        std::string
        Path::GetDirectoryName() const
        {
            ASSERT(IsDirectory());
            const auto dir_char = absolute_path_.rfind(
                    DirectoryChar(), absolute_path_.length() - 2);

            if(dir_char == std::string::npos)
            {
                // this is a root file, so return root directory
                return absolute_path_;
            }

            return absolute_path_.substr(dir_char);
        }

        std::string
        Path::GetFileName() const
        {
            ASSERTX(IsFile(), absolute_path_);
            const auto dir_char = absolute_path_.rfind(
                    DirectoryChar(), absolute_path_.length() - 2);

            if(dir_char == std::string::npos)
            {
                // this is a root file, so return root directory
                return absolute_path_;
            }

            return absolute_path_.substr(dir_char + 1);
        }

        bool
        Path::operator==(const Path& rhs) const
        {
            return absolute_path_ == rhs.absolute_path_;
        }

        Path::Path(const std::string& absolute_path)
            : absolute_path_(absolute_path)
        {}

    }  // namespace vfs

}  // namespace euphoria::core
