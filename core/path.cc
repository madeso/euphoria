#include "core/path.h"

#include "core/assert.h"
#include "core/stringutils.h"

const char
DIRECTORY_CHAR()
{
  return '/';
}
std::string
DIRECTORY_STRING()
{
  return "/";
}

Path
Path::FromDirectory(const std::string& path)
{
  if(path.empty() || EndsWith(path, DIRECTORY_STRING()))
    return Path{path};
  return Path{path + DIRECTORY_STRING()};
}

Path
Path::FromRoot()
{
  return Path{""};
}


Path
Path::FromFile(const std::string& path)
{
  return Path{path};
}

Path
Path::FromGuess(const std::string& path)
{
  return Path{path};
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
    const auto dir_char = absolute_path_.rfind(DIRECTORY_CHAR());
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
  if(!IsDirectory())
    return FromDirectory("not a directory");
  // ASSERT(IsDirectory());

  const auto dir_char =
      absolute_path_.rfind(DIRECTORY_CHAR(), absolute_path_.length() - 2);
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
  ASSERT(name.find(DIRECTORY_CHAR()) == std::string::npos);

  return FromDirectory(absolute_path_ + name);
}

const Path
Path::GetFile(const std::string& name) const
{
  ASSERT(IsDirectory());
  ASSERT(!EndsWith(name, DIRECTORY_STRING()));

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

Path::Path(const std::string& absolute_path)
    : absolute_path_(absolute_path)
{
}
