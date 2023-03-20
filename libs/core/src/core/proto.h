#pragma once



#include <variant>
#include <optional>

#include "assert/assert.h"

#include "gaf/lib_gaf.h"
#include "rapidjson/document.h"

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FilePath;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Result types

    struct ReadErrorFileMissing
    {
        std::string path_to_file;
        std::string error_for_debugging;

        ReadErrorFileMissing(const std::string&, const std::string&);
    };

    struct ReadErrorFileError
    {
        std::string path_to_file;
        std::vector<std::string> errors;

        explicit ReadErrorFileError(const std::string&);
        ReadErrorFileError(const std::string&, const std::vector<std::string>&);
    };

    template<typename T>
    using ReadResult = std::variant<T, ReadErrorFileMissing, ReadErrorFileError>;



    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Helper functions

    std::string get_string_from_path(const vfs::FilePath& p);
    std::string get_string_from_path_for_debugging(vfs::FileSystem* fs, const vfs::FilePath& p);
    std::optional<std::string> get_file_contents_or_null(vfs::FileSystem* fs, const vfs::FilePath& file_name);
    std::optional<std::string> read_source_or_get_error_message(const std::string& source, rapidjson::Document* doc);
    std::string could_be_callback(const std::string& v, const std::vector<std::string>& vv);
    
    void log_read_error(const ReadErrorFileMissing&);
    void log_read_error(const ReadErrorFileError&);

    
    template<typename> inline constexpr bool always_false_v = false;

    template<typename T>
    std::optional<T> get_optional_and_log_errors_impl(ReadResult<T>&& result, bool log_missing_file)
    {
        try
        {
            return std::visit
            (
                [log_missing_file](auto&& arg) -> std::optional<T>
                {
                    using TArg = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<TArg, T>)
                    {
                        return arg;
                    }
                    else if constexpr (std::is_same_v<TArg, ReadErrorFileMissing>)
                    {
                        if(log_missing_file)
                        {
                            log_read_error(arg);
                        }

                        return std::nullopt;
                    }
                    else if constexpr (std::is_same_v<TArg, ReadErrorFileError>)
                    {
                        log_read_error(arg);
                        return std::nullopt;
                    }
                    else
                    {
                        static_assert(always_false_v<TArg>, "non-exhaustive visitor!");
                    }
                },
                result
            );
        }
        catch(const std::bad_variant_access&)
        {
            DIE("invalid state of ReadResult");
            return std::nullopt;
        }
    }



    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Usage helpers

    // log all errors, only return when file loaded
    template<typename T>
    std::optional<T> get_optional_and_log_errors(ReadResult<T>&& result)
    {
        return get_optional_and_log_errors_impl<T>(std::move(result), true);
    }

    // don't log error and return default when file is missing
    // log syntax error
    template<typename T>
    std::optional<T> get_optional_and_log_errors_allow_missing(ReadResult<T>&& result)
    {
        // todo(Gustav): should this return something different to indicate that file was missing?
        return get_optional_and_log_errors_impl<T>(std::move(result), false);
    }

    // todo(Gustav): remove all get_default and add proper error handling to callers

    template<typename T>
    T get_default_but_log_errors(ReadResult<T>&& result)
    {
        auto r = get_optional_and_log_errors_impl<T>(std::move(result), true);
        if(r)
        {
            return std::move(*r);
        }
        else
        {
            return T{};
        }
    }

    template<typename T>
    T get_default_ignore_missing_but_log_errors(ReadResult<T>&& result)
    {
        auto r = get_optional_and_log_errors_impl<T>(std::move(result), false);
        if(r)
        {
            return std::move(*r);
        }
        else
        {
            return T{};
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Actual load functions

    template <typename T, typename TReadJsonElementFunc>
    ReadResult<T> read_json_source_to_gaf_struct
    (
        const std::string& path_to_file,
        const std::string& source,
        TReadJsonElementFunc read_json_element
    )
    {
        rapidjson::Document doc;
        auto error_message = read_source_or_get_error_message(source, &doc);
        if(error_message)
        {
            // json parse error
            return ReadErrorFileError
            {
                path_to_file,
                std::vector<std::string>
                {
                    std::move(*error_message)
                }
            };
        }
        else
        {
            std::vector<gaf::Error> errors;
            auto result = read_json_element
            (
                &errors,
                doc,
                could_be_callback,
                "$"
            );
            if(errors.empty() == false)
            {
                auto ret = ReadErrorFileError{path_to_file};
                for(const auto& e: errors)
                {
                    // todo(Gustav): handle error return better... copy first error or xpath to clipboard?
                    ret.errors.emplace_back(e.description);
                }
                return ret;
            }
            if(result)
            {
                return *result;
            }

            auto ret = ReadErrorFileError{path_to_file};
            ret.errors.emplace_back("XML ok but unknown error occured");
            return ret;
        }
    }

    template <typename T, typename TReadJsonElementFunc>
    ReadResult<T> read_json_file_to_gaf_struct
    (
        vfs::FileSystem* fs,
        const vfs::FilePath& file_name,
        TReadJsonElementFunc read_json_element
    )
    {
        auto source = get_file_contents_or_null(fs, file_name);
        if(source)
        {
            return read_json_source_to_gaf_struct
            <T, TReadJsonElementFunc>
            (
                get_string_from_path(file_name),
                *source,
                read_json_element
            );
        }
        else
        {
            return ReadErrorFileMissing
            {
                get_string_from_path(file_name),
                get_string_from_path_for_debugging(fs, file_name)
            };
        }
    }
}
