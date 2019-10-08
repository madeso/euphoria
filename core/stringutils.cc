#include "core/stringutils.h"

#include "core/assert.h"

#include <string>
#include <algorithm>
#include <cstring>
#include <vector>
#include <sstream>
#include <iterator>

namespace euphoria::core
{
    std::pair<std::string, std::string>
    LastStrings(const std::string& str, char sep)
    {
        auto result = str.find(sep);
        if(result == std::string::npos)
        {
            return std::make_pair(str, "");
        }

        const auto parent = str.substr(0, result);
        const auto child  = str.substr(result, str.length() - parent.length());
        return std::make_pair(parent, child);
    }

    std::string
    StripLastString(const std::string& str, char sep)
    {
        auto result = str.find(sep);
        if(result == std::string::npos)
        {
            return "";
        }

        return str.substr(0, result);
    }

    std::string
    TrimRight(
            const std::string& string_to_trim,
            const std::string& trim_characters)
    {
        return std::string(string_to_trim)
                .erase(string_to_trim.find_last_not_of(trim_characters) + 1);
    }

    std::string
    TrimLeft(
            const std::string& string_to_trim,
            const std::string& trim_characters)
    {
        return std::string(string_to_trim)
                .erase(0, string_to_trim.find_first_not_of(trim_characters));
    }

    std::string
    Trim(const std::string& string_to_trim, const std::string& trim_characters)
    {
        return TrimRight(
                TrimLeft(string_to_trim, trim_characters), trim_characters);
    }

    bool
    StartsWith(const std::string& string_to_test, const std::string& start)
    {
        const std::string::size_type length       = start.length();
        const std::string::size_type other_length = string_to_test.length();
        if(other_length < length)
        {
            return false;
        }
        const std::string actual_start = string_to_test.substr(0, length);
        return start == actual_start;
    }

    bool
    EndsWith(const std::string& string_to_test, const std::string& end)
    {
        const std::string::size_type length       = end.length();
        const std::string::size_type other_length = string_to_test.length();
        if(other_length < length)
        {
            return false;
        }
        const std::string actual_end
                = string_to_test.substr(other_length - length, length);
        return end == actual_end;
    }

    char
    ToLower(char b)
    {
        if(b >= 'A' && b <= 'Z')
            return b - 'A' + 'a';
        return b;
    }

    // std::transform call in the ToLower (string) function
    // can't figure out we want the ToLower(char) function so
    // we create a custom function to call the regular function
    inline char
    ToLowerChar(char b)
    {
        return ToLower(b);
    }

    std::string
    ToLower(const std::string& str)
    {
        std::string result = str;
        std::transform(
                result.begin(), result.end(), result.begin(), ToLowerChar);
        return result;
    }

    std::vector<std::string>
    ToLower(const std::vector<std::string>& str)
    {
        return VectorToStringVector(
                str, [](const std::string& s) { return ToLower(s); });
    }

    std::string
    ToUpper(const std::string& str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), toupper);
        return result;
    }

    void
    StringReplace(
            std::string*       string,
            const std::string& to_find,
            const std::string& to_replace)
    {
        std::size_t       index       = string->find(to_find);
        const std::size_t find_length = to_find.length();
        ASSERT(find_length > 0);
        while(index != std::string::npos)
        {
            string->erase(index, find_length);
            string->insert(index, to_replace);
            index = string->find(to_find, index);
        }
    }

    const std::string
    StringReplace(
            const std::string& string,
            const std::string& to_find,
            const std::string& to_replace)
    {
        std::string temp = string;
        StringReplace(&temp, to_find, to_replace);
        return temp;
    }

    void
    Copy(char* dst, const std::string& src, const std::string::size_type& count)
    {
        strncpy(dst, src.c_str(), count - 1);
        dst[count - 1] = 0;
    }

    std::string
    ReplaceWithCharacter(
            const std::string& string,
            const std::string& to_find,
            const char         to_replace)
    {
        std::string s = string;
        for(char c: to_find)
        {
            std::replace(s.begin(), s.end(), c, to_replace);
        }
        return s;
    }

    std::string
    RemoveFromEnd(const std::string& str, const std::string& end)
    {
        if(EndsWith(str, end))
        {
            const auto new_length = str.length() - end.length();
            if(new_length == 0)
            {
                return "";
            }
            ASSERT(new_length > 0);
            return str.substr(0, new_length);
        }

        return str;
    }

    std::string
    Strip(const std::string& str, const std::string& ch)
    {
        std::stringstream ss;
        for(const char c: str)
        {
            if(ch.find(c) == std::string::npos)
            {
                ss << c;
            }
        }
        return ss.str();
    }

    // remove all characters in ch except the first one in a chain from str
    std::string
    RemoveConsecutive(const std::string& str, const std::string& ch)
    {
        std::stringstream ss;
        bool              skip = false;
        for(const char c: str)
        {
            if(ch.find(c) == std::string::npos)
            {
                ss << c;
                skip = false;
            }
            else
            {
                if(!skip)
                {
                    ss << c;
                    skip = true;
                }
            }
        }
        return ss.str();
    }

    template <typename Out>
    void
    Split(const std::string& s, char delim, Out result)
    {
        std::stringstream ss(s);
        std::string       item;
        while(std::getline(ss, item, delim))
        {
            *(result++) = item;
        }
    }

    std::vector<std::string>
    Split(const std::string& s, char delim)
    {
        std::vector<std::string> elems;
        Split(s, delim, std::back_inserter(elems));
        return elems;
    }

    const std::string&
    OpString(bool b, const std::string& str)
    {
        if(b)
        {
            return str;
        }
        else
        {
            static const std::string Empty = "";
            return Empty;
        }
    }


    char
    IsNumber(char b)
    {
        if(b >= '0' && b <= '9')
            return 1;
        return 0;
    }

    int
    ParseNumber(const char*& a)
    {
        int result = *a - '0';
        ++a;

        while(IsNumber(*a))
        {
            result *= 10;
            result += *a - '0';
            ++a;
        }

        --a;
        return result;
    }

    int
    StringCompare(const std::string& lhs, const std::string& rhs)
    {
        const auto* a = lhs.c_str();
        const auto* b = rhs.c_str();

        if(a == b)
            return 0;

        if(a == NULL)
            return -1;
        if(b == NULL)
            return 1;

        while(*a && *b)
        {
            int a0, b0;  // will contain either a number or a letter

            if(IsNumber(*a))
            {
                a0 = ParseNumber(a) + 256;
            }
            else
            {
                a0 = ToLower(*a);
            }
            if(IsNumber(*b))
            {
                b0 = ParseNumber(b) + 256;
            }
            else
            {
                b0 = ToLower(*b);
            }

            if(a0 < b0)
                return -1;
            if(a0 > b0)
                return 1;

            ++a;
            ++b;
        }

        if(*a)
            return 1;
        if(*b)
            return -1;

        return 0;
    }

}  // namespace euphoria::core
