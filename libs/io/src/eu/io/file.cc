#include "eu/io/file.h"

#include <fstream>

namespace eu::io
{

    
std::string string_from_stream(std::istream& handle)
{
    // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring/2602258
    std::string line;
    std::ostringstream ss;
    while (std::getline(handle, line))
    {
        ss << line << '\n';
    }
    return ss.str();
}

std::string string_from_file(const std::string& path)
{
    std::ifstream handle(path);
    if (handle.good() == false)
    {
        LOG_ERR("Unable to open file '{}'", path);
        return "";
    }

    return string_from_stream(handle);
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
