#pragma once

#include <string>
#include <vector>

#include "pugixml.hpp"

namespace euphoria::core
{
    namespace vfs
    {
        struct file_system;
        struct file_path;
    }

    std::string
    read_file_or_get_error_message
    (
        vfs::file_system* fs,
        pugi::xml_document* doc,
        const vfs::file_path& file_name
    );

    std::string
    read_source_or_get_error_message(const std::string& source, pugi::xml_document* doc);

    std::string
    could_be(const std::string& v, const std::vector<std::string>& vv);

    // return error message or empty
    template <typename T>
    std::string
    read_json_to_gaf_struct_or_get_error_message(vfs::file_system* fs, T* message, const vfs::file_path& file_name)
    {
        pugi::xml_document doc;
        const std::string r = read_file_or_get_error_message(fs, &doc, file_name);
        if(r.empty())
        {
            // assume there is a gaf function for the struct we are using...
            return ReadXmlElement(message, doc.document_element(), could_be);
        }
        else
        {
            return r;
        }
    }

    template <typename T>
    std::string
    read_xml_source_to_gaf_struct_or_get_error_message(const std::string& source, T* message)
    {
        pugi::xml_document doc;
        const std::string r = read_source_or_get_error_message(source, &doc);
        if(r.empty())
        {
            // assume there is a gaf function for the struct we are using...
            return ReadXmlElement(message, doc.document_element(), could_be);
        }
        else
        {
            return r;
        }
    }


}
