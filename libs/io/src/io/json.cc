// Copyright (2015) Gustav

#include "io/json.h"

#include "assert/assert.h"
#include "log/log.h"

#include "base/editdistance.search.h"
#include "base/stringmerger.h"
#include "base/functional.h"


#include "io/vfs.h"
#include "io/vfs_path.h"

#include <fstream>

namespace eu::io
{
    std::string
    get_string_from_path_for_debugging(FileSystem* fs, const FilePath& p)
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
        return fmt::format("File {} was requrested with roots: {}. All entries: {}. Did you mean {}?", p, fs->get_roots_as_string(), all_entries, closest);
    }

    JsonResult
    read_json_file
    (
        FileSystem* fs,
        const FilePath& file_name
    )
    {
        const auto source = fs->read_file_to_string(file_name);
        if (!source)
        {
            return JsonResult::create_error({JsonErrorType::file_error, get_string_from_path_for_debugging(fs, file_name)});
        }

        const auto result = jsonh::Parse(*source, jsonh::parse_flags::Json);
        if (!result)
        {
            std::ostringstream ss;
            ss << "Parse error:\n";
            for (const auto& err : result.errors)
            {
                const auto& loc = err.location;
                ss << "file("
                    << loc.line << ":"
                    << loc.column << "): "
                    << err.message << "\n";
            }
            return JsonResult::create_error({JsonErrorType::parse_error, ss.str()});
        }

        return JsonResult::create_value({result.doc, *result.root});
    }

    std::string
    could_be(const std::string& invalid_value, const std::vector<std::string>& possible_values)
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


    std::optional<jsonh::Value>
    ObjectQuery::get(const std::string& name)
    {
        const auto ret = object->object.find(name);

        if (ret != object->object.end())
        {
            found.emplace(name);
            return ret->second;
        }
        else
        {
            missing.emplace(name);
            return std::nullopt;
        }
    }

    ObjectQuery::ObjectQuery(const jsonh::Object* o)
        : object(o)
    {
    }

    std::optional<std::string>
    ObjectQuery::get_missing_errors_message()
    {
        std::vector<std::string> errors;

        const auto miss = std::vector<std::string>(missing.begin(), missing.end());

        for (const auto& x : object->object)
        {
            const auto& key = x.first;
            if (found.find(key) != found.end()) { continue; }

            // todo(Gustav): add file, line and column
            errors.emplace_back(fmt::format("existing key {} was not read, could be {}", key, could_be(key, miss)));
        }

        for (const auto& r : required)
        {
            errors.emplace_back(fmt::format("required key {} was missing from json object", r));
        }

        if (errors.empty()) { return std::nullopt; }

        // todo(Gustav): join with newlines instead?
        else { return string_mergers::english_and.merge(errors); }
    }
}

