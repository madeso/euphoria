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
        vfs::FileSystem*     fs,
        rapidjson::Document* doc,
        const vfs::FilePath& file_name
    )
    {
        std::string source;

        const bool load_result = fs->ReadFileToString(file_name, &source);
        if(!load_result)
        {
            return Str() << "Unable to load file " << file_name << " from "
                         << fs->GetRootsAsString();
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
        InputStream             stream {ss};
        constexpr unsigned int  ParseFlags
                = rapidjson::kParseCommentsFlag
                  | rapidjson::kParseTrailingCommasFlag;
        doc->ParseStream<ParseFlags, rapidjson::UTF8<>, InputStream>(stream);

        if(doc->HasParseError())
        {
            // todo(Gustav): add file and parse error to error
            return Str {} << "JSON parse error(" << stream.GetLine() << ":"
                          << stream.GetColumn() << "): "
                          << rapidjson::GetParseError_En(doc->GetParseError());
        }

        return "";
    }

}  // namespace euphoria::core
