#pragma once

#include <string>
#include "rapidjson/document.h"

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FilePath;
    }

    std::string
    read_file_or_get_error_message
    (
        vfs::FileSystem* fs,
        rapidjson::Document* doc,
        const vfs::FilePath& file_name
    );

    std::string
    read_source_or_get_error_message(const std::string& source, rapidjson::Document* doc);

    // return error message or empty
    template <typename T>
    std::string
    read_json_to_gaf_struct_or_get_error_message(vfs::FileSystem* fs, T* message, const vfs::FilePath& file_name)
    {
        rapidjson::Document doc;
        const std::string r = read_file_or_get_error_message(fs, &doc, file_name);
        if(r.empty())
        {
            // assume there is a gaf function for the struct we are using...
            return ReadFromJsonValue(message, doc, "");
        }
        else
        {
            return r;
        }
    }


}
