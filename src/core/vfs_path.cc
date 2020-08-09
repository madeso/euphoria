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


        std::optional<FilePath>
        FilePath::FromScript(const std::string& p)
        {
            if(p.find('\\') != std::string::npos)
            {
                return std::nullopt;
            }
            if(p.find(':') != std::string::npos)
            {
                return std::nullopt;
            }
            if(StartsWith(p, "~/") || StartsWith(p, "./"))
            {
                return FilePath{p};
            }
            else
            {
                if(StartsWith(p, "/"))
                {
                    return FilePath{"." + p};
                }
                else
                {
                    return FilePath{"./" + p};
                }
            }
        }


        std::optional<FilePath>
        FilePath::FromDirtySource(const std::string& p)
        {
            std::string s = Trim(p);
            if(s.size() < 4)
            {
                return std::nullopt;
            }
            // slashes
            s = ReplaceWithCharacter(s, "\\", '/');

            // C:\style paths
            if(s.substr(1, 2) == ":/")
            {
                s = s.substr(1);
                s[0] = '.';
            }

            // \\share\style paths
            if(s.substr(0, 2) == "//")
            {
                s[0] = '.';
            }
            return FromScript(s);
        }


        std::optional<FilePath>
        FilePath::FromScriptOrEmpty(const std::string& path)
        {
            if(path.empty())
            {
                return std::nullopt;
            }
            else
            {
                return FromScript(path);
            }
        }


        std::tuple<DirPath, std::string>
        FilePath::SplitDirectoriesAndFile() const
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
            return {DirPath{dir_part}, file_part};
        }


        DirPath
        FilePath::GetDirectory() const
        {
            return std::get<0>(SplitDirectoriesAndFile());
        }


        std::string
        FilePath::GetFileWithExtension() const
        {
            return std::get<1>(SplitDirectoriesAndFile());
        }


        std::string
        FilePath::GetFilenameWithoutExtension() const
        {
            const auto with_extension = GetFileWithExtension();
            const auto dot = with_extension.find('.', 1);
            if(dot == std::string::npos) { return with_extension; }
            return with_extension.substr(0, dot);
        }


        std::string
        FilePath::GetExtension() const
        {
            const auto with_extension = GetFileWithExtension();
            const auto dot = with_extension.find('.', 1);
            if(dot == std::string::npos) { return ""; }
            return with_extension.substr(dot+1);
        }

        FilePath
        FilePath::SetExtensionCopy(const std::string& ext) const
        {
            ASSERT(!ext.empty());
            ASSERTX(ext[0]!='.' && *ext.rbegin()!='.', ext);
            const auto slash = path.rfind('/');
            ASSERTX(slash != std::string::npos, path, ext);
            const auto dot = path.find('.', slash);
            if(dot == std::string::npos)
            {
                return FilePath{path + "." + ext};
            }
            else
            {
                return FilePath{path.substr(0, dot+1) + ext};
            }
        }


        FilePath
        FilePath::ExtendExtensionCopy(const std::string& ext) const
        {
            ASSERT(!ext.empty());
            ASSERTX(ext[0]!='.' && *ext.rbegin()!='.', ext);
            const auto old_ext = GetExtension();
            if(old_ext.empty())
            {
                return SetExtensionCopy(ext);
            }
            else
            {
                return SetExtensionCopy(old_ext + "." + ext);
            }
        }


        FilePath::FilePath(const std::string& p)
            : path(p)
        {
            ASSERTX(path.size() > 3, path);
            ASSERTX((path[0] == '~' || path[0] == '.') && path[1] == '/', path);
            ASSERTX(*path.rbegin() != '/', path);
        }


        // --------------------------------------------------------------------
        

        DirPath
        DirPath::FromRoot()
        {
            return DirPath{"~/"};
        }


        DirPath
        DirPath::FromRelative()
        {
            return DirPath{"./"};
        }


        DirPath
        DirPath::FromDirs(const std::vector<std::string>& dirs)
        {
            ASSERT(!dirs.empty());
            ASSERTX(IsValidFirstDirectory(dirs[0]), dirs[0]);
            for(std::size_t index=1; index<dirs.size(); index +=1)
            {
                ASSERTX(IsValidDirectoryName(dirs[index]), dirs[index]);
            }

            return DirPath
            {
                StringMerger{}
                    .Separator("/")
                    .StartAndEnd("", "/")
                    .Generate(dirs)
            };
        }


        FilePath
        DirPath::GetFile(const std::string& filename) const
        {
            ASSERTX(IsValidFilename(filename), path, filename);
            return FilePath{path + filename};
        }


        bool
        DirPath::IsRelative() const
        {
            ASSERT(!path.empty());
            const auto first = path[0];
            ASSERTX(first == '.' || first == '~', first);
            return first == '.';
        }


        bool
        DirPath::ContainsRelative() const
        {
            const auto is_relative = [](const std::string& dir) -> bool
            {
                if(dir == ".") { return true; }
                if(dir == "..") { return true; }

                return false;
            };
            const auto dirs = SplitDirectories();

            for(const auto& d: dirs)
            {
                if(is_relative(d))
                {
                    return true;
                }
            }

            return false;
        }


        DirPath
        DirPath::GetParentDirectory() const
        {
            auto dirs = SplitDirectories();
            ASSERTX
            (
                !dirs.empty(),
                StringMerger::Array().Generate(dirs)
            );
            dirs.pop_back();
            return DirPath::FromDirs(dirs);
        }


        std::string
        DirPath::GetDirectoryName() const
        {
            const auto dirs = SplitDirectories();
            ASSERTX(dirs.size() > 1, StringMerger::Array().Generate(dirs));
            return *dirs.rbegin();
        }


        std::vector<std::string>
        DirPath::SplitDirectories() const
        {
            return Split(path, '/');
        }


        DirPath
        DirPath::SingleCdCopy(const std::string& single) const
        {
            ASSERTX(IsValidDirectoryName(single), single);
            return DirPath{path + single + "/"};
        }


        DirPath::DirPath(const std::string& p)
            : path(p)
        {
            ASSERT(!path.empty());
            ASSERTX(IsValidFirstDirectory(SplitDirectories()[0]), path);
            ASSERTX(*path.rbegin() == '/', path);
        }


        // --------------------------------------------------------------------


        std::optional<DirPath>
        ResolveRelative(const DirPath& base)
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

            return DirPath::FromDirs(dirs);
        }


        std::optional<DirPath>
        ResolveRelative(const DirPath& base, const DirPath& root)
        {
            ASSERTX(root.IsRelative() == false, root);
            const auto path
                = base.IsRelative()
                ? Join(root, base)
                : base;
            return ResolveRelative(path);
        }


        std::optional<FilePath>
        ResolveRelative(const FilePath& base)
        {
            const auto [dir, file] = base.SplitDirectoriesAndFile();
            const auto resolved = ResolveRelative(dir);
            if(!resolved.has_value()) { return std::nullopt; }
            return resolved.value().GetFile(file);
        }

        std::optional<FilePath>
        ResolveRelative(const FilePath& base, const DirPath& root)
        {
            const auto [dir, file] = base.SplitDirectoriesAndFile();
            const auto resolved = ResolveRelative(dir, root);
            if(!resolved.has_value()) { return std::nullopt; }
            return resolved.value().GetFile(file);
        }


        DirPath
        Join(const DirPath& lhs, const DirPath& rhs)
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


        FilePath
        Join(const DirPath& lhs, const FilePath& rhs)
        {
            const auto [dir, file] = rhs.SplitDirectoriesAndFile();
            const auto joined = Join(lhs, dir);
            return joined.GetFile(file);
        }


        bool
        operator==(const DirPath& lhs, const DirPath& rhs)
        {
            return lhs.path == rhs.path;
        }


        bool
        operator==(const FilePath& lhs, const FilePath& rhs)
        {
            return lhs.path == rhs.path;
        }


        bool
        operator!=(const DirPath& lhs, const DirPath& rhs)
        {
            return lhs.path != rhs.path;
        }


        bool
        operator!=(const FilePath& lhs, const FilePath& rhs)
        {
            return lhs.path != rhs.path;
        }


        bool
        operator<(const DirPath& lhs, const DirPath& rhs)
        {
            return lhs.path < rhs.path;
        }


        bool
        operator<(const FilePath& lhs, const FilePath& rhs)
        {
            return lhs.path < rhs.path;
        }
        
    }  // namespace vfs
}  // namespace euphoria::core
