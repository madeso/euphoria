#include "core/os.h"
#include "core/stringutils.h"

// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
// look into using https://github.com/cxong/tinydir instead of current platform
// hack...?

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// copied from https://stackoverflow.com/a/145309/180307

#include <cstdio> /* defines FILENAME_MAX */
#include <utility>
#ifdef _MSC_VER
#include "windows.h"
#include <direct.h>
#define GET_CURRENT_DIR _getcwd

#ifdef GetCurrentDirectory
#undef GetCurrentDirectory
#endif
#else
#include <dirent.h>
#include <unistd.h>
#include <vector>

#define GET_CURRENT_DIR getcwd
#endif

namespace euphoria::core
{
    std::string
    get_current_directory()
    {
        char current_directory[FILENAME_MAX];

        if(!GET_CURRENT_DIR(
                   static_cast<char*>(current_directory),
                   sizeof(current_directory)))
        {
            return "";
        }

        current_directory[sizeof(current_directory) - 1] = 0;

        const std::string ret = static_cast<char*>(current_directory);
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

    directory_listing
    list_directory(const std::string& path)
    {
        const std::string search_path = string_builder() << path << "*.*";
        WIN32_FIND_DATA   fd;
        HANDLE            hFind = ::FindFirstFile(search_path.c_str(), &fd);


        if(hFind != INVALID_HANDLE_VALUE)
        {
            directory_listing ret;
            ret.valid = true;

            do
            {
                // read all (real) files in current folder, delete '!' read other 2
                // default folder . and ..
                if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    ret.files.push_back(fd.cFileName);
                }
                else  // Put folders into vector
                {
                    ret.directories.push_back(fd.cFileName);
                }
            } while(::FindNextFile(hFind, &fd));
            ::FindClose(hFind);

            return ret;
        }
        else
        {
            directory_listing ret;
            ret.valid = false;
            return ret;
        }
    }
#else
    directory_listing
    list_directory(const std::string& path)
    {
        struct dirent* ent = nullptr;

        DIR* dir = opendir(path.c_str());
        if(dir != nullptr)
        {
            directory_listing ret;
            ret.valid = true;

            /* print all the files and directories within directory */
            while((ent = readdir(dir)) != nullptr)
            {
                const std::string name = ent->d_name;
                if(ent->d_type == DT_REG)
                {
                    ret.files.emplace_back(name);
                }
                else if(ent->d_type == DT_DIR)
                {
                    if(name == "." || name == "..")
                    {
                    }
                    else
                    {
                        ret.directories.emplace_back(name);
                    }
                }
            }

            closedir(dir);
            return ret;
        }
        else
        {
            directory_listing ret;
            ret.valid = false;
            return ret;
        }
    }
#endif

    // todo(Gustav): move to stringutils
    bool
    ends_with(const std::string& str, char c)
    {
        const auto l = str.length();

        if(l == 0)
        {
            return false;
        }

        return str[l - 1] == c;
    }

    std::string
    join_path(const std::string& left, const std::string& right)
    {
        if(ends_with(left, path_separator))
        {
            return left + right;
        }
        else
        {
            return left + path_separator + right;
        }
    }

    std::string
    get_extension(const std::string& path)
    {
        return last_strings(path, '.').second;
    }

    std::string
    get_file_name_including_extension(const std::string& path)
    {
        const auto r = last_strings(path, path_separator).second;
        if(r.empty())
        {
            return r;
        }
        else
        {
            // skip leading path_separator
            return r.substr(1);
        }
    }

    std::string
    get_file_name_without_extension(const std::string& path)
    {
        return last_strings(get_file_name_including_extension(path), '.').first;
    }

}
