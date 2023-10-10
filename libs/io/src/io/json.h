#pragma once

#include <optional>
#include <set>

#include "jsonh/jsonh.h"

#include "log/log.h"

#include "assert/assert.h"

#include "base/result.h"


namespace eu::io
{
    struct FileSystem;
    struct FilePath;

    struct Json { jsonh::Document doc; jsonh::Value root; };
    enum class JsonErrorType { file_error, parse_error };
    struct JsonError { JsonErrorType error; std::string display; };

    using JsonResult = Result<Json, JsonError>;

    std::string get_string_from_path_for_debugging(FileSystem* fs, const FilePath& p);

    JsonResult
    read_json_file
    (
        FileSystem* fs,
        const FilePath& file_name
    );

    std::string could_be(const std::string& invalid_value, const std::vector<std::string>& possible_values);

    struct ObjectQuery
    {
        explicit ObjectQuery(const jsonh::Object* o);

        const jsonh::Object* object;

        std::set<std::string> found; // all found props
        std::set<std::string> missing; // all missed props
        std::vector<std::string> required; // all missed and required props

        std::optional<jsonh::Value> get(const std::string& name);

        std::optional<std::string> get_missing_errors_message();
    };
}

#define JSON_PARSE_FUNC(TYPE) \
    bool parse(::eu::log::Logger* log, TYPE* owner, jsonh::Value root_val, const jsonh::Document* doc)

#define JSON_EXPECT(x, msg) \
    do {\
        if (!(x))\
        {\
            log->error(msg);\
            return false;\
        }\
    } while(false)

#define JSON_BEGIN_ENUM(TYPE) \
    const auto* dnu_val = root_val.AsString(doc);\
    JSON_EXPECT(dnu_val, "Expected string value for " #TYPE);\
    const std::string dnu_enum_name = #TYPE; \
    using dnu_enum_type = TYPE; \
    std::vector<std::string> dnu_values

#define JSON_ENUM_VAL(VAL) \
    if(dnu_val->value == #VAL) {\
        *owner = dnu_enum_type::VAL;\
        return true; \
    } \
    else { \
        dnu_values.emplace_back(#VAL);\
    }
#define JSON_END_ENUM() \
    log->error(fmt::format("{} was invalid for {}, could be {}",\
        dnu_val->value, dnu_enum_name,\
        eu::io::could_be(dnu_val->value, dnu_values)));\
    return false

#define JSON_BEGIN_OBJECT() \
    const auto* root = root_val.AsObject(doc);\
    JSON_EXPECT(root, "root is not a object");\
    auto object = ::eu::io::ObjectQuery{ root }

#define JSON_VAL_X(TYPE, PROP, REQUIRED) \
    do {\
        auto dnu_json = object.get(#PROP);\
        if (dnu_json)\
        {\
            auto* dnu_prop = dnu_json->As ## TYPE(doc);\
            JSON_EXPECT(dnu_prop, #PROP " is not a " #TYPE);\
            owner->PROP = dnu_prop->value;\
        }\
        else if constexpr(REQUIRED)\
        {\
            object.required.emplace_back(#PROP);\
        }\
    } while(false)

#define JSON_VAL_OBJ_X(TYPE, PROP, REQUIRED, CON) \
    do {\
        auto dnu_json = object.get(#PROP);\
        if (dnu_json)\
        {\
            TYPE dnu_prop = {};\
            if (false == parse(log, &dnu_prop, *dnu_json, doc))\
            {\
                return false;\
            }\
            owner->PROP = CON(std::move(dnu_prop));\
        }\
        else if constexpr(REQUIRED)\
        {\
            object.required.emplace_back(#PROP);\
        }\
    } while(false)

#define JSON_VAL_OBJ(TYPE, PROP) JSON_VAL_OBJ_X(TYPE, PROP, true, )
#define JSON_VAL_OPT_OBJ(TYPE, PROP) JSON_VAL_OBJ_X(TYPE, PROP, false, )
#define JSON_VAL_OPT_OBJ_CON(TYPE, PROP, CON) JSON_VAL_OBJ_X(TYPE, PROP, false, CON)

#define JSON_VAL(TYPE, PROP) JSON_VAL_X(TYPE, PROP, true)
#define JSON_OPT_VAL(TYPE, PROP) JSON_VAL_X(TYPE, PROP, false)

// todo(Gustv): remove expect in favor of if statements, logging and setting a failure state so this node is loaded completly before aborting
#define JSON_ARRAY(TYPE, PROP) \
    do{\
        auto dnu_val = object.get(#PROP);\
        JSON_EXPECT(dnu_val, "missing property " #PROP);\
        auto* dnu_array = dnu_val->AsArray(doc);\
        JSON_EXPECT(dnu_array, #PROP " is not a array");\
        for (const auto dnu_json : dnu_array->array)\
        {\
            TYPE dnu_prop;\
            if (false == parse(log, &dnu_prop, dnu_json, doc))\
            {\
                return false;\
            }\
            owner->PROP.emplace_back(std::move(dnu_prop));\
        }\
    } while(false)

#define JSON_ARRAY_PROP_CON(TYPE, PROP, CON) \
    do{\
        auto dnu_val = object.get(#PROP);\
        JSON_EXPECT(dnu_val, "missing property " #PROP);\
        auto* dnu_array = dnu_val->AsArray(doc);\
        JSON_EXPECT(dnu_array, #PROP " is not a array");\
        for (const auto dnu_json : dnu_array->array)\
        {\
            const auto* dnu_prop = dnu_json.As ## TYPE(doc);\
            JSON_EXPECT(dnu_prop, #PROP " is not a " #TYPE);\
            owner->PROP.emplace_back(CON(dnu_prop->value));\
        }\
    } while(false)

#define JSON_ARRAY_PROP(TYPE, PROP) \
    JSON_ARRAY_PROP_CON(TYPE, PROP, )

#define JSON_END_OBJECT() \
    const auto missing = object.get_missing_errors_message();\
    if (missing)\
    {\
        log->error(*missing);\
        return false;\
    }\
    return true

