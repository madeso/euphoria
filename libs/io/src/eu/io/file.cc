#include "eu/io/file.h"

#include <fstream>

namespace eu::io
{

    template<typename TChar>
std::basic_string<TChar> tstring_from_stream(std::basic_istream<TChar>& handle)
{
    // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring/2602258
    std::basic_string<TChar> line;
    std::basic_ostringstream<TChar> ss;
    while (std::getline(handle, line))
    {
        ss << line << '\n';
    }
    return ss.str();
}

template<typename TChar>
std::basic_string<TChar> tstring_from_file(const std::string& path, std::basic_string<TChar> err)
{
    std::basic_ifstream<TChar> handle(path);
    if (handle.good() == false)
    {
        LOG_ERR("Unable to open file '{}'", path);
        return err;
    }

    return tstring_from_stream<TChar>(handle);
}

std::u8string u8string_from_file(const std::string& path)
{
    return tstring_from_file<char8_t>(path, u8"");
}

std::string string_from_file(const std::string& path)
{
    return tstring_from_file<char>(path, "");
}

std::vector<uint8_t> bytes_from_file(const std::string& path)
{
    std::ifstream stream(path, std::ios::in | std::ios::binary);
    if (stream.good() == false)
    {
        LOG_ERR("Unable to open file '{}'", path);
        return {};
    }
    std::vector<uint8_t> contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    return contents;
}


}
