// Copyright (2015) Gustav

#include "core/proto.h"

#include "core/assert.h"
#include "core/vfs.h"
#include "core/str.h"
#include "core/vfs_path.h"

#include "gaf/lib_pugixml.h"

#include <fstream>

namespace euphoria::core
{
    std::string
    read_file_or_get_error_message
    (
        vfs::file_system* fs,
        pugi::xml_document* doc,
        const vfs::file_path& file_name
    )
    {
        std::string source;

        const bool load_result = fs->read_file_to_string(file_name, &source);
        if(!load_result)
        {
            return string_builder() << "Unable to load file " << file_name << " from "
                         << fs->get_roots_as_string();
        }

        return read_source_or_get_error_message(source, doc);
    }

    std::string
    could_be(const std::string& v, const std::vector<std::string>& vv)
    {
        return gaf::could_be_fun_all(v, vv);
    }

    struct Location
    {
        int line = 1;
        int offset = 0;
    };

    Location get_location_from_offset(const std::string& source, std::size_t offset)
    {
        Location r;

        for(std::size_t i=0; i<offset; i+=1)
        {
            if(source[i] == '\n')
            {
                r.line += 1;
                r.offset = 0;
            }
            else
            {
                r.offset += 1;
            }
        }

        return r;
    }

    std::string
    read_source_or_get_error_message(const std::string& source, pugi::xml_document* doc)
    {
        // todo(Gustav): look up insitu parsing
        // todo(Gustav): look upo json/sjson parsing options

        const auto result = doc->load_buffer(source.data(), source.length());

        if(result.status != pugi::status_ok)
        {
            const auto location = get_location_from_offset(source, result.offset);
            // todo(Gustav): add file and parse error to error
            return string_builder {}
                << "XML error: " << result.description() << "\n"
                << "Error offset: (" << location.line << ":" << location.offset << ") (error at [..." << source.substr(result.offset, 10) << "]";
        }

        return "";
    }

}
