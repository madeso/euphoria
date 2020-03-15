#ifndef CORE_ENUM_TO_STRING_H
#define CORE_ENUM_TO_STRING_H

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <ostream>

#include "core/assert.h"
#include "core/stringutils.h"
#include "core/editdistance.h"

#include "magic_enum/magic_enum.hpp"


namespace euphoria::core
{
    template <typename T>
    struct MatchedEnum
    {
        bool single_match = false;
        std::vector<std::string> names;
        std::vector<T> values;
    };

    template <typename T>
    struct EnumToStringImpl
    {
        std::map<T, std::string> enum_to_string;
        std::map<std::string, T> string_to_enum;

        int size = 0;

        // todo: replace with initializer list?
        EnumToStringImpl<T>&
        Add(const std::string& name, T t)
        {
            enum_to_string.insert(std::make_pair(t, name));
            string_to_enum.insert(std::make_pair(ToLower(name), t));
            size += 1;
            return *this;
        }

        std::string
        ToString(T t) const
        {
            auto found = enum_to_string.find(t);
            if(found != enum_to_string.end())
            {
                return found->second;
            }
            DIE("Enum not added");
            return "???";
        }

        std::vector<std::string>
        ListNames() const
        {
            std::vector<std::string> ret;
            for(auto entry: enum_to_string)
            {
                ret.emplace_back(entry.second);
            }
            return ret;
        }

        std::vector<T>
        ListValues() const
        {
            std::vector<T> ret;
            for(auto entry: enum_to_string)
            {
                ret.emplace_back(entry.first);
            }
            return ret;
        }

        MatchedEnum<T>
        Match(const std::string& input, size_t max_size) const
        {
            auto found = string_to_enum.find(ToLower(input));
            if(found != string_to_enum.end())
            {
                return MatchedEnum<T> {true, {input}, {found->second}};
            }
            struct Match
            {
                std::string name;
                T t;
                unsigned long changes;

                bool
                operator<(const Match& rhs) const
                {
                    return changes < rhs.changes;
                }
            };
            std::priority_queue<Match> matches;
            for(auto entry: enum_to_string)
            {
                const auto t = entry.first;
                const auto str = entry.second;
                const auto changes = EditDistance(str, input);
                matches.push({str, t, changes});
                if(matches.size() > max_size)
                {
                    matches.pop();
                }
            }
            auto ret = MatchedEnum<T> {};
            while(!matches.empty())
            {
                ret.names.insert(ret.names.begin(), matches.top().name);
                ret.values.insert(ret.values.begin(), matches.top().t);
                matches.pop();
            }
            return ret;
        }
    };

    template <typename T>
    std::string
    EnumToString(T t)
    {
        return std::string{ magic_enum::enum_name(t) };
    }

    template<typename T>
    EnumToStringImpl<T>
    EnumToStringImplFromEnum()
    {
        const auto values = magic_enum::enum_values<T>();
        
        EnumToStringImpl<T> r;
        for (const auto v : values)
        {
            r.Add(EnumToString(v), v);
        }

        return r;
    }


    template <typename T>
    MatchedEnum<T>
    StringToEnum(const std::string& input, size_t max_size = 5)
    {
        return EnumToStringImplFromEnum<T>().Match(input, max_size);
    }

    
    template <typename T>
    std::vector<std::string>
    EnumToString(const std::vector<T>& ts)
    {
        std::vector<std::string> ret;
        for(auto t: ts)
        {
            ret.emplace_back(EnumToString<T>(t));
        }
        return ret;
    }


    template <typename T>
    std::vector<std::string>
    EnumToString()
    {
        return EnumToStringImplFromEnum<T>().ListNames();
    }

    template <typename T>
    std::vector<T>
    EnumValues()
    {
        const auto values = magic_enum::enum_values<T>();
        return values;
    }
}

using namespace magic_enum::ostream_operators;

#endif  // CORE_ENUM_TO_STRING_H
