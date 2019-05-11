#ifndef EUPHORIA_PATH_H
#define EUPHORIA_PATH_H

#include <string>

namespace vfs
{

// represents a virtual path
// always lowercase
// forward slash specify a directory
// todo: transform into a path and directory class instead
class Path
{
 public:
  static Path
  FromDirectory(const std::string& path);
  static Path
  FromRoot();
  static Path
  FromFile(const std::string& path);
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

#endif  // EUPHORIA_PATH_H
