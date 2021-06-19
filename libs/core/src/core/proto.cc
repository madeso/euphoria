// Copyright (2015) Gustav

#include "core/proto.h"

#include "core/assert.h"
#include "core/vfs.h"
#include "core/str.h"
#include "core/vfs_path.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stream.h"
#include "rapidjson/cursorstreamwrapper.h"

#include <fstream>

namespace euphoria::core
{
    std::string
    read_file_or_get_error_message
    (
        vfs::file_system* fs,
        rapidjson::Document* doc,
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
    read_source_or_get_error_message(const std::string& source, rapidjson::Document* doc)
    {
        // todo(Gustav): look up insitu parsing
        // todo(Gustav): look upo json/sjson parsing options

        using InputStream
                = rapidjson::CursorStreamWrapper<rapidjson::StringStream>;
        rapidjson::StringStream ss {source.c_str()};
        InputStream stream {ss};
        constexpr unsigned int parse_flags
                = rapidjson::kParseCommentsFlag
                  | rapidjson::kParseTrailingCommasFlag;
        doc->ParseStream<parse_flags, rapidjson::UTF8<>, InputStream>(stream);

        if(doc->HasParseError())
        {
            // todo(Gustav): add file and parse error to error
            return string_builder {} << "JSON parse error(" << stream.GetLine() << ":"
                          << stream.GetColumn() << "): "
                          << rapidjson::GetParseError_En(doc->GetParseError());
        }

        return "";
    }

}
