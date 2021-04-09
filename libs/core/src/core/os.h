#ifndef EUPHORIA_OS_H
#define EUPHORIA_OS_H

#include <string>
#include <vector>


namespace euphoria::core
{
    std::string
    GetCurrentDirectory();


    struct DirectoryList
    {
        std::vector<std::string> files;
        std::vector<std::string> directories;
        bool                     valid;
    };

    DirectoryList
    ListDirectory(const std::string& path);

    const char PATH_SEPARATOR = '/';

    bool
    EndsWith(const std::string& str, char c);

    std::string
    JoinPath(const std::string& left, const std::string& right);

    std::string
    GetExtension(const std::string& path);

    std::string
    GetFileNameIncludingExtension(const std::string& path);

    std::string
    GetFileNameWithoutExtension(const std::string& path);

}  // namespace euphoria::core

#endif  // EUPHORIA_OS_H
