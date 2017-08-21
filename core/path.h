#ifndef EUPHORIA_PATH_H
#define EUPHORIA_PATH_H

#include <string>

class Path
{
 public:
  static Path
  FromDirectory(const std::string& path);
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
  GetSubDirectory(const std::string& name);
  const Path
  GetFile(const std::string& name);

  bool
  IsDirectory() const;
  bool
  IsFile() const;

  const std::string&
  GetAbsolutePath() const;

 private:
  Path(const std::string& absolute_path);
  std::string absolute_path_;
};


#endif  // EUPHORIA_PATH_H
