#include "core/vfs_path.h"

#include "core/assert.h"
#include "core/stringutils.h"
#include "core/stringmerger.h"


namespace euphoria::core
{
    namespace vfs
    {
        namespace
        {
            bool
            IsValidDirectoryName(const std::string& dir)
            {
                if(dir.empty()) { return false; }
                if(dir == ".") { return true; }
                if(dir == "..") { return true; }
                if(dir.find('.') != std::string::npos) { return false; }
                if(dir.find('/') != std::string::npos) { return false; }
                return true;
            }

            bool
            IsValidFirstDirectory(const std::string& dir)
            {
                if(dir == "~") { return true; }
                if(dir == ".") { return true; }
                return false;
            }

            bool
            IsValidFilename(const std::string& file)
            {
                if(file.empty()) { return false; }
                if(file.find('/') != std::string::npos) { return false; }
                if(file == ".") { return false; }
                return true;
            }
        }


        std::tuple<PathToDirectory, std::string>
        PathToFile::SplitDirectoriesAndFile() const
        {
            const auto slash = path.rfind('/');
            ASSERTX(slash != std::string::npos, path);
            const auto dir_part = path.substr(0, slash+1);
            const auto file_part = path.substr(slash+1);
            ASSERTX
            (
                !file_part.empty() && !dir_part.empty(),
                path, dir_part, file_part
            );
            ASSERTX
            (
                *dir_part.rbegin() == '/' && file_part[0] != '/',
                path, dir_part, file_part
            );
            return {PathToDirectory{dir_part}, file_part};
        }


        PathToDirectory
        PathToFile::GetDirectory() const
        {
            return std::get<0>(SplitDirectoriesAndFile());
        }


        std::string
        PathToFile::GetFileWithExtension() const
        {
            return std::get<1>(SplitDirectoriesAndFile());
        }


        std::string
        PathToFile::GetFilenameWithoutExtension() const
        {
            const auto with_extension = GetFileWithExtension();
            const auto dot = with_extension.find('.', 1);
            if(dot == std::string::npos) { return with_extension; }
            return with_extension.substr(0, dot);
        }


        std::string
        PathToFile::GetExtension() const
        {
            const auto with_extension = GetFileWithExtension();
            const auto dot = with_extension.find('.', 1);
            if(dot == std::string::npos) { return ""; }
            return with_extension.substr(dot+1);
        }


        PathToFile::PathToFile(const std::string& p)
            : path(p)
        {
            ASSERTX(path.size() > 3, path);
            ASSERTX((path[0] == '~' || path[0] == '.') && path[1] == '/', path);
            ASSERTX(*path.rbegin() != '/', path);
        }


        // --------------------------------------------------------------------
        

        PathToDirectory
        PathToDirectory::FromRoot()
        {
            return PathToDirectory{"~/"};
        }


        PathToDirectory
        PathToDirectory::FromRelative()
        {
            return PathToDirectory{"./"};
        }


        PathToDirectory
        PathToDirectory::FromDirs(const std::vector<std::string>& dirs)
        {
            ASSERT(!dirs.empty());
            ASSERTX(IsValidFirstDirectory(dirs[0]), dirs[0]);
            for(std::size_t index=1; index<dirs.size(); index +=1)
            {
                ASSERTX(IsValidDirectoryName(dirs[index]), dirs[index]);
            }

            return PathToDirectory
            {
                StringMerger{}
                    .Separator("/")
                    .StartAndEnd("", "/")
                    .Generate(dirs)
            };
        }


        PathToFile
        PathToDirectory::GetFile(const std::string& filename) const
        {
            ASSERTX(IsValidFilename(filename), path, filename);
            return PathToFile{path + filename};
        }


        bool
        PathToDirectory::IsRelative() const
        {
            ASSERT(!path.empty());
            const auto first = path[0];
            ASSERTX(first == '.' || first == '~', first);
            return first == '.';
        }


        bool
        PathToDirectory::ContainsRelative() const
        {
            const auto is_relative = [](const std::string& dir) -> bool
            {
                if(dir == ".") { return true; }
                if(dir == "..") { return true; }

                return false;
            };
            const auto dirs = SplitDirectories();

            for(const auto d: dirs)
            {
                if(is_relative(d))
                {
                    return true;
                }
            }

            return false;
        }


        std::vector<std::string>
        PathToDirectory::SplitDirectories() const
        {
            return Split(path, '/');
        }


        PathToDirectory
        PathToDirectory::SingleCdCopy(const std::string& single) const
        {
            ASSERTX(IsValidDirectoryName(single), single);
            return PathToDirectory{path + single + "/"};
        }


        PathToDirectory::PathToDirectory(const std::string& p)
            : path(p)
        {
            ASSERT(!path.empty());
            ASSERTX(IsValidFirstDirectory(SplitDirectories()[0]), path);
            ASSERTX(*path.rbegin() == '/', path);
        }


        // --------------------------------------------------------------------


        std::optional<PathToDirectory>
        ResolveRelative
        (
            const PathToDirectory& base
        )
        {
            ASSERT(!base.IsRelative());

            auto dirs = base.SplitDirectories();
            auto size = dirs.size();
            
            if(size == 1) { return base; }

            for(std::size_t index=1; index<size;)
            {
                if(dirs[index] != "..")
                {
                    index += 1;
                }
                else
                {
                    // if it is the first directory, then this is an error
                    // since we can't back out of the root and the
                    // path is actually invalid, like:
                    // ~/../outside_root/
                    if(index == 1) { return std::nullopt; }

                    // drop .. and the parent folder from the path
                    dirs.erase
                    (
                        dirs.begin() + (index-1),
                        dirs.begin() + (index+1)
                    );
                    index -= 1;
                    size -= 2;
                }
            }

            return PathToDirectory::FromDirs(dirs);
        }


        std::optional<PathToDirectory>
        ResolveRelative
        (
            const PathToDirectory& base,
            const PathToDirectory& root
        )
        {
            ASSERT(base.IsRelative());
            return ResolveRelative(Join(root, base));
        }


        std::optional<PathToFile>
        ResolveRelative
        (
            const PathToFile& base
        )
        {
            const auto [dir, file] = base.SplitDirectoriesAndFile();
            const auto resolved = ResolveRelative(dir);
            if(!resolved.has_value()) { return std::nullopt; }
            return resolved.value().GetFile(file);
        }

        std::optional<PathToFile>
        ResolveRelative
        (
            const PathToFile& base,
            const PathToDirectory& root
        )
        {
            const auto [dir, file] = base.SplitDirectoriesAndFile();
            const auto resolved = ResolveRelative(dir, root);
            if(!resolved.has_value()) { return std::nullopt; }
            return resolved.value().GetFile(file);
        }


        PathToDirectory
        Join(const PathToDirectory& lhs, const PathToDirectory& rhs)
        {
            ASSERT(rhs.IsRelative());
            const auto dirs = rhs.SplitDirectories();

            auto ret = lhs;
            for(size_t index = 1; index < dirs.size(); index +=1 )
            {
                ret = ret.SingleCdCopy(dirs[index]);
            }

            return ret;
        }


        PathToFile
        Join(const PathToDirectory& lhs, const PathToFile& rhs)
        {
            const auto [dir, file] = rhs.SplitDirectoriesAndFile();
            const auto joined = Join(lhs, dir);
            return joined.GetFile(file);
        }


        //
        //---------------------------------------------------------------------
        // old part
        //---------------------------------------------------------------------
        //

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
