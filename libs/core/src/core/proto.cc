// Copyright (2015) Gustav

#include "core/proto.h"

#include "assert/assert.h"
#include "core/vfs.h"
#include "core/vfs_path.h"
#include "log/log.h"
#include "core/editdistance.search.h"
#include "core/stringmerger.h"
#include "core/functional.h"

#include "rapidjson/error/en.h"

#include <fstream>

namespace euphoria::core
{
    std::string
    get_string_from_path(const vfs::FilePath& p)
    {
        return "{}"_format(p);
    }

    std::string
    get_string_from_path_for_debugging(vfs::FileSystem* fs, const vfs::FilePath& p)
    {
        // todo(Gustav): walk vfs tree to display what part of the file exists
        // For example:
        //     ~/folder/dir/file.txt is requested
        //     dir/file.txt is missing in ~/folder/
        //     or
        //     ~/folder/[dir/file.txt] exists...

        constexpr std::size_t max_items = 3;

        const auto files = fs->list_files(p.get_directory());
        const auto file_name = p.get_file_with_extension();
        const auto values = map<std::string>
        (
            search::find_closest<search::Match>
            (
                max_items, files,
                [&](const auto& value) -> search::Match
                {
                    // todo(Gustav): ignore directories?
                    return {value.name, file_name};
                }
            ),
            [](const search::Match& m)
            {
                return m.name;
            }
        );

        const auto all_entries = string_mergers::array.merge(map<std::string>(files, [](const auto& f) { return f.name; }));
        const auto closest = string_mergers::english_or.merge(values);
        return "File {} is requrested with roots: {}. All entries: {}. Did you mean {}?"_format(p, fs->get_roots_as_string(), all_entries, closest);
    }

    std::optional<std::string>
    get_file_contents_or_null
    (
        vfs::FileSystem* fs,
        const vfs::FilePath& file_name
    )
    {
        std::string source;
        const bool file_loaded = fs->read_file_to_string(file_name, &source);

        if(file_loaded)
        {
            return source;
        }
        else
        {
            return std::nullopt;
        }
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

    constexpr unsigned rapid_json_flags = rapidjson::kParseNoFlags
            | rapidjson::kParseValidateEncodingFlag // Validate encoding of JSON strings.
            | rapidjson::kParseCommentsFlag // Allow one - line(//) and multi-line (/**‍/) comments.
            | rapidjson::kParseTrailingCommasFlag // Allow trailing commas at the end of objects and arrays.
            | rapidjson::kParseEscapedApostropheFlag // Allow escaped apostrophe in strings.
        ;

    std::optional<std::string>
    read_source_or_get_error_message(const std::string& source, rapidjson::Document* doc)
    {
        // todo(Gustav): look up insitu & sax parsing

        doc->Parse<rapid_json_flags>(source.c_str());

        const auto err = doc->GetParseError();
        if (err != rapidjson::kParseErrorNone)
        {
            const auto error_offset = doc->GetErrorOffset();
            const auto location = get_location_from_offset(source, error_offset);
            // todo(Gustav): make the source offset string better looking, like a compiler error with a "here: ^~~~~~~" text
            return
                "JSON error: {}\n"
                "Error offset: ({}:{}) (error at [...{}]"_format
                (
                    rapidjson::GetParseError_En(err),
                    location.line,
                    location.offset,
                    source.substr(error_offset, 10)
                );
        }

        return std::nullopt;
    }

    
    std::string
    could_be_callback(const std::string& invalid_value, const std::vector<std::string>& possible_values)
    {
        constexpr std::size_t max_items = 3;
        const auto values = map<std::string>
        (
            search::find_closest<search::Match>
            (
                max_items, possible_values,
                [&](const auto& value) -> search::Match
                {
                    return {value, invalid_value};
                }
            ),
            [](const search::Match& m)
            {
                return m.name;
            }
        );
        return string_mergers::english_or.merge(values);
    }

    ReadErrorFileMissing::ReadErrorFileMissing(const std::string& p, const std::string& e)
        : path_to_file(p)
        , error_for_debugging(e)
    {
    }

    ReadErrorFileError::ReadErrorFileError(const std::string& p)
        : path_to_file(p)
    {
    }

    ReadErrorFileError::ReadErrorFileError(const std::string& p, const std::vector<std::string>& e)
        : path_to_file(p)
        , errors(e)
    {
    }

    void log_read_error(const ReadErrorFileMissing& missing)
    {
        LOG_ERROR("Missing file {0}: {1}", missing.path_to_file, missing.error_for_debugging);
    }

    void log_read_error(const ReadErrorFileError& error)
    {
        for(const auto& err: error.errors)
        {
            LOG_ERROR("{0}: {1}", error.path_to_file, err);
        }
        LOG_ERROR("{0} error(s) found in {1}", error.errors.size(), error.path_to_file);
    }
}
