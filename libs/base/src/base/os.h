#pragma once





namespace eu
{
    std::string
    get_current_directory();


    struct DirectoryListing
    {
        std::vector<std::string> files;
        std::vector<std::string> directories;
        bool valid = false;
    };

    DirectoryListing
    list_directory(const std::string& path);

    constexpr char path_separator = '/';

    bool
    ends_with(const std::string& str, char c);

    std::string
    join_path(const std::string& left, const std::string& right);

    std::string
    get_extension(const std::string& path);

    std::string
    get_file_name_including_extension(const std::string& path);

    std::string
    get_file_name_without_extension(const std::string& path);

}

