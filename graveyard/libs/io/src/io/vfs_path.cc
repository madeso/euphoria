#include "io/vfs_path.h"

#include <algorithm>
#include "assert/assert.h"

#include "base/cint.h"
#include "base/stringutils.h"
#include "base/stringmerger.h"


namespace eu::io
{
    namespace
    {
        bool
        is_valid_directory_name(const std::string& dir)
        {
            if(dir.empty()) { return false; }
            if(dir == ".") { return true; }
            if(dir == "..") { return true; }
            if(dir.find('.') != std::string::npos) { return false; }
            if(dir.find('/') != std::string::npos) { return false; }
            return true;
        }

        bool
        is_valid_first_directory(const std::string& dir)
        {
            if(dir == "~") { return true; }
            if(dir == ".") { return true; }
            return false;
        }

        bool
        is_valid_filename(const std::string& file)
        {
            if(file.empty()) { return false; }
            if(file.find('/') != std::string::npos) { return false; }
            if(file == ".") { return false; }
            return true;
        }
    }


    std::optional<FilePath>
    FilePath::from_script(const std::string& p)
    {
        if(p.find('\\') != std::string::npos)
        {
            return std::nullopt;
        }
        if(p.find(':') != std::string::npos)
        {
            return std::nullopt;
        }
        if(begins_with(p, "~/") || begins_with(p, "./"))
        {
            return FilePath{p};
        }
        else
        {
            if(begins_with(p, "/"))
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
    FilePath::from_dirty_source(const std::string& p)
    {
        std::string s = trim(p);
        if(s.size() < 4)
        {
            return std::nullopt;
        }
        // slashes
        s = replace_with_character(s, "\\", '/');

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
        return from_script(s);
    }


    std::optional<FilePath>
    FilePath::from_script_or_empty(const std::string& path)
    {
        if(path.empty())
        {
            return std::nullopt;
        }
        else
        {
            return from_script(path);
        }
    }


    std::tuple<DirPath, std::string>
    FilePath::split_directories_and_file() const
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
    FilePath::get_directory() const
    {
        return std::get<0>(split_directories_and_file());
    }


    std::string
    FilePath::get_file_with_extension() const
    {
        return std::get<1>(split_directories_and_file());
    }


    std::string
    FilePath::get_filename_without_extension() const
    {
        const auto with_extension = get_file_with_extension();
        const auto dot = with_extension.find('.', 1);
        if(dot == std::string::npos) { return with_extension; }
        return with_extension.substr(0, dot);
    }


    std::string
    FilePath::get_extension() const
    {
        const auto with_extension = get_file_with_extension();
        const auto dot = with_extension.find('.', 1);
        if(dot == std::string::npos) { return ""; }
        return with_extension.substr(dot+1);
    }

    FilePath
    FilePath::set_extension_copy(const std::string& ext) const
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
    FilePath::extend_extension_copy(const std::string& ext) const
    {
        ASSERT(!ext.empty());
        ASSERTX(ext[0]!='.' && *ext.rbegin()!='.', ext);
        const auto old_ext = get_extension();
        if(old_ext.empty())
        {
            return set_extension_copy(ext);
        }
        else
        {
            return set_extension_copy(old_ext + "." + ext);
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
    DirPath::from_root()
    {
        return DirPath{"~/"};
    }


    DirPath
    DirPath::from_relative()
    {
        return DirPath{"./"};
    }


    DirPath
    DirPath::from_dirs(const std::vector<std::string>& dirs)
    {
        ASSERT(!dirs.empty());
        ASSERTX(is_valid_first_directory(dirs[0]), dirs[0]);
        for(std::size_t index=1; index<dirs.size(); index +=1)
        {
            ASSERTX(is_valid_directory_name(dirs[index]), dirs[index]);
        }

        return DirPath
        {
            StringMerger{}
                .set_separator("/")
                .set_start_and_end("", "/")
                .merge(dirs)
        };
    }


    FilePath
    DirPath::get_file(const std::string& filename) const
    {
        ASSERTX(is_valid_filename(filename), path, filename);
        return FilePath{path + filename};
    }


    bool
    DirPath::is_relative() const
    {
        ASSERT(!path.empty());
        const auto first = path[0];
        ASSERTX(first == '.' || first == '~', first);
        return first == '.';
    }


    bool
    DirPath::contains_relative() const
    {
        const auto is_relative = [](const std::string& dir) -> bool
        {
            if(dir == ".") { return true; }
            if(dir == "..") { return true; }

            return false;
        };

        const auto dirs = split_directories();
        return std::any_of(dirs.begin(), dirs.end(), is_relative);
    }


    DirPath
    DirPath::get_parent_directory() const
    {
        auto dirs = split_directories();
        ASSERTX
        (
            !dirs.empty(),
            string_mergers::array.merge(dirs)
        );
        dirs.pop_back();
        return DirPath::from_dirs(dirs);
    }


    std::string
    DirPath::get_name() const
    {
        const auto dirs = split_directories();
        ASSERTX(dirs.size() > 1, string_mergers::array.merge(dirs));
        return *dirs.rbegin();
    }


    std::vector<std::string>
    DirPath::split_directories() const
    {
        auto r = split(path, '/');

        // if path ends with / then remove the empty "folder"
        if(r.empty() == false && r.rbegin()->empty())
        {
            r.pop_back();
        }
        return r;
    }

    std::string to_string(const FilePath& p)
        { return p.path; }

    std::string to_string(const DirPath& p)
        { return p.path; }


    DirPath
    DirPath::get_directory(const std::string& single) const
    {
        ASSERTX(is_valid_directory_name(single), single);
        return DirPath{path + single + "/"};
    }


    DirPath::DirPath(const std::string& p)
        : path(p)
    {
        ASSERT(!path.empty());
        ASSERTX(is_valid_first_directory(split_directories()[0]), path);
        ASSERTX(*path.rbegin() == '/', path);
    }


    // --------------------------------------------------------------------


    std::optional<DirPath>
    resolve_relative(const DirPath& base)
    {
        ASSERT(!base.is_relative());

        auto dirs = base.split_directories();
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
                    std::next(dirs.begin(), c_sizet_to_int(index)-1),
                    std::next(dirs.begin(), c_sizet_to_int(index)+1)
                );
                index -= 1;
                size -= 2;
            }
        }

        return DirPath::from_dirs(dirs);
    }


    std::optional<DirPath>
    resolve_relative(const DirPath& base, const DirPath& root)
    {
        ASSERTX(root.is_relative() == false, root);
        const auto path
            = base.is_relative()
            ? join(root, base)
            : base;
        return resolve_relative(path);
    }


    std::optional<FilePath>
    resolve_relative(const FilePath& base)
    {
        const auto [dir, file] = base.split_directories_and_file();
        const auto resolved = resolve_relative(dir);
        if(!resolved.has_value()) { return std::nullopt; }
        return resolved.value().get_file(file);
    }

    std::optional<FilePath>
    resolve_relative(const FilePath& base, const DirPath& root)
    {
        const auto [dir, file] = base.split_directories_and_file();
        const auto resolved = resolve_relative(dir, root);
        if(!resolved.has_value()) { return std::nullopt; }
        return resolved.value().get_file(file);
    }


    DirPath
    join(const DirPath& lhs, const DirPath& rhs)
    {
        ASSERT(rhs.is_relative());
        const auto dirs = rhs.split_directories();

        auto ret = lhs;
        for(size_t index = 1; index < dirs.size(); index +=1 )
        {
            ret = ret.get_directory(dirs[index]);
        }

        return ret;
    }


    FilePath
    join(const DirPath& lhs, const FilePath& rhs)
    {
        const auto [dir, file] = rhs.split_directories_and_file();
        const auto joined = join(lhs, dir);
        return joined.get_file(file);
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

}
