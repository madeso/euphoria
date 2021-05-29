#pragma once

#include <string>
#include <map>
#include <vector>
#include <ostream>

#include "core/assert.h"
#include "core/stringutils.h"
#include "core/editdistance.search.h"

#include "magic_enum/magic_enum.hpp"


namespace euphoria::core
{
    template <typename T>
    struct matched_enum
    {
        bool single_match = false;
        std::vector<std::string> names;
        std::vector<T> values;
    };

    template <typename T>
    struct enum_to_string_implementation
    {
        std::map<T, std::string> enum_to_string;
        std::map<std::string, T> string_to_enum;

        int size = 0;

        // todo(Gustav): replace with initializer list?
        enum_to_string_implementation<T>&
        add(const std::string& name, T t)
        {
            enum_to_string.insert(std::make_pair(t, name));
            string_to_enum.insert(std::make_pair(to_lower(name), t));
            size += 1;
            return *this;
        }

        [[nodiscard]] std::string
        to_string(T t) const
        {
            auto found = enum_to_string.find(t);
            if(found != enum_to_string.end())
            {
                return found->second;
            }
            DIE("Enum not added");
            return "???";
        }

        [[nodiscard]] std::vector<std::string>
        get_list_of_names() const
        {
            std::vector<std::string> ret;
            for(auto entry: enum_to_string)
            {
                ret.emplace_back(entry.second);
            }
            return ret;
        }

        [[nodiscard]] std::vector<T>
        get_list_of_values() const
        {
            std::vector<T> ret;
            for(auto entry: enum_to_string)
            {
                ret.emplace_back(entry.first);
            }
            return ret;
        }

        [[nodiscard]] matched_enum<T>
        match(const std::string& input, size_t max_size) const
        {
            auto found = string_to_enum.find(to_lower(input));
            if(found != string_to_enum.end())
            {
                return matched_enum<T> {true, {input}, {found->second}};
            }

            struct match : search::match
            {
                T t;

                match(const std::string& str, T tt, const std::string& input)
                    : search::match(str, input)
                    , t(tt)
                {
                }
            };

            const auto matches = search::find_closest<match>
            (
                max_size, enum_to_string,
                [&](const auto& entry) -> match
                {
                    return {entry.second, entry.first, input};
                }
            );

            auto ret = matched_enum<T> {};
            for(const auto& m: matches)
            {
                ret.names.emplace_back(m.name);
                ret.values.emplace_back(m.t);
            }
            return ret;
        }
    };

    template <typename T>
    [[nodiscard]] std::string
    enum_to_string(T t)
    {
        return std::string{ magic_enum::enum_name(t) };
    }

    template<typename T>
    [[nodiscard]] enum_to_string_implementation<T>
    get_enum_to_string_implementation_from_enum()
    {
        const auto values = magic_enum::enum_values<T>();

        enum_to_string_implementation<T> r;
        for (const auto v : values)
        {
            r.add(enum_to_string(v), v);
        }

        return r;
    }


    template <typename T>
    matched_enum<T>
    string_to_enum(const std::string& input, size_t max_size = 5)
    {
        return get_enum_to_string_implementation_from_enum<T>().match(input, max_size);
    }


    template <typename T>
    [[nodiscard]] std::vector<std::string>
    enum_to_string(const std::vector<T>& ts)
    {
        std::vector<std::string> ret;
        for(auto t: ts)
        {
            ret.emplace_back(enum_to_string<T>(t));
        }
        return ret;
    }


    template <typename T>
    [[nodiscard]] std::vector<std::string>
    enum_to_string()
    {
        return get_enum_to_string_implementation_from_enum<T>().get_list_of_names();
    }

    template <typename T>
    [[nodiscard]] std::vector<T>
    enum_values()
    {
        const auto values = magic_enum::enum_values<T>();
        return values;
    }
}

using namespace magic_enum::ostream_operators; // NOLINT

