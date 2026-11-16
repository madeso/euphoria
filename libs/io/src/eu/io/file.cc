#include "eu/io/file.h"

#include <fstream>

namespace internal
{
    namespace
    {
        template<typename TChar>
        std::basic_string<TChar> tstring_from_stream(std::basic_istream<TChar>& handle)
        {
            // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring/2602258
            return { std::istreambuf_iterator<TChar>(handle), std::istreambuf_iterator<TChar>() };
        }

        template<typename TChar>
        std::basic_string<TChar> tstring_from_file(const std::string& path, std::basic_string<TChar> err)
        {
            std::basic_ifstream<TChar> handle(path, std::ios::in | std::ios::binary);
            if (handle.good() == false)
            {
                LOG_ERR("Unable to open file '{}'", path);
                return err;
            }

            const auto content = tstring_from_stream<TChar>(handle);
            if (handle.bad())
            {
                LOG_ERR("Failed to read file '{}'", path);
                return err;
            }

            return content;
        }
    }
}

namespace eu::io
{
std::string string_from_file(const std::string& path)
{
    return internal::tstring_from_file<char>(path, "");
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
    if (stream.bad())
    {
        LOG_ERR("Failed to read file '{}'", path);
        return {};
    }

    return contents;
}


}
