#include "core/os.h"
#include "core/stringutils.h"

#include <dirent.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// copied from https://stackoverflow.com/a/145309/180307

#include <cstdio> /* defines FILENAME_MAX */
#include <utility>
#ifdef _MSC_VER
#include <direct.h>
#define GET_CURRENT_DIR _getcwd
#else
#include <unistd.h>
#include <vector>

#define GET_CURRENT_DIR getcwd
#endif

std::string
GetCurrentDirectory()
{
  char current_directory[FILENAME_MAX];

  if(!GET_CURRENT_DIR(
         static_cast<char*>(current_directory), sizeof(current_directory)))
  {
    return "";
  }

  current_directory[sizeof(current_directory) - 1] = 0;

  const std::string ret = static_cast<char*>(current_directory);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////

DirectoryList
ListDirectory(const std::string& path)
{
  DIR*           dir;
  struct dirent* ent;
  dir = opendir(path.c_str());
  if(dir != nullptr)
  {
    DirectoryList ret;
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
    DirectoryList ret;
    ret.valid = false;
    return ret;
  }
}

bool
EndsWith(const std::string& str, char c)
{
  const auto l = str.length();

  if(l == 0)
  {
    return false;
  }

  return str[l - 1] == c;
}

std::string
JoinPath(const std::string& left, const std::string& right)
{
  if(EndsWith(left, PATH_SEPARATOR))
  {
    return left + right;
  }
  else
  {
    return left + PATH_SEPARATOR + right;
  }
}

std::string
GetExtension(const std::string& path)
{
  return LastStrings(path, '.').second;
}

std::string
GetFileNameIncludingExtension(const std::string& path)
{
  const auto r = LastStrings(path, PATH_SEPARATOR).second;
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
GetFileNameWithoutExtension(const std::string& path)
{
  return LastStrings(GetFileNameIncludingExtension(path), '.').first;
}
