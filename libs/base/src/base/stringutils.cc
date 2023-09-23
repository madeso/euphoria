#include "base/stringutils.h"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <type_traits>

#include "assert/assert.h"

#include "base/stringbuilder.h"


namespace eu
{

std::pair<std::string, std::string>
get_last_string(const std::string& str, char sep)
{
    auto result = str.find(sep);
    if(result == std::string::npos)
    {
        return std::make_pair(str, "");
    }

    const auto parent = str.substr(0, result);
    const auto child = str.substr(result, str.length() - parent.length());
    return std::make_pair(parent, child);
}


std::string
get_first_chars(const std::string& str, std::size_t count)
{
    if(str.length() < count) { return str; }
    else { return str.substr(0, count); }
}


std::string
get_first_chars_with_ellipsis(const std::string& str, unsigned int count)
{
    if (str.length() > count)
    {
        return str.substr(0, count) + "...";
    }

    return str;
}


std::string
strip_last_string(const std::string& str, char sep)
{
    auto result = str.find(sep);
    if(result == std::string::npos)
    {
        return "";
    }

    return str.substr(0, result);
}


std::string
trim_right(const std::string& string_to_trim, const std::string& trim_characters)
{
    return std::string(string_to_trim).erase(string_to_trim.find_last_not_of(trim_characters) + 1);
}


std::string
trim_left(const std::string& string_to_trim, const std::string& trim_characters)
{
    return std::string(string_to_trim).erase(0, string_to_trim.find_first_not_of(trim_characters));
}


std::string
trim(const std::string& string_to_trim, const std::string& trim_characters)
{
    return trim_right(trim_left(string_to_trim, trim_characters), trim_characters);
}


// todo(Gustav): rename to begins_with
bool
begins_with(const std::string& string_to_test, const std::string& start)
{
    const std::string::size_type length = start.length();
    const std::string::size_type other_length = string_to_test.length();
    if(other_length < length)
    {
        return false;
    }
    const std::string actual_start = string_to_test.substr(0, length);
    return start == actual_start;
}


bool
ends_with(const std::string& string_to_test, const std::string& end)
{
    const std::string::size_type length = end.length();
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
to_lower_char(char b)
{
    if(b >= 'A' && b <= 'Z')
    {
        return static_cast<char>((static_cast<int>(b) - 'A') + 'a');
    }
    else
    {
        return b;
    }
}

char
to_upper_char(char b)
{
    if(b >= 'a' && b <= 'z')
    {
        return static_cast<char>(b + ('A' - 'a'));
    }
    else
    {
        return b;
    }
}


std::string
to_lower(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), to_lower_char);
    return result;
}


std::vector<std::string>
to_lower(const std::vector<std::string>& str)
{
    return to_string_vector(str, [](const std::string& s) { return to_lower(s); });
}


std::string
to_upper(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), to_upper_char);
    return result;
}


std::string
from_char_to_string(char c, CharToStringStyle style)
{
    const auto name = [&]() -> std::string
    {
        switch (c)
        {
        case 0: return "<null>";
        case '\n': return "<\\n>";
        case '\r': return "<\\r>";
        case '\t': return "<tab>";
        // source: http://www.asciitable.com/
        case  1: return "<start of heading>";
        case  2: return "<start of text>";
        case  3: return "<end of text>";
        case  4: return "<end of transmission>";
        case  5: return "<enquiry>";
        case  6: return "<acknowledge>";
        case  7: return "<bell>";
        case  8: return "<backspace>";
        // case  9: return "<horizontal tab>";
        // case 10: return "<newline>";
        case 11: return "<vertical tab>";
        case 12: return "<new page>";
        // case 13: return "<carriage return>";
        case 14: return "<shift out>";
        case 15: return "<shift in>";
        case 16: return "<data link esqape>";
        case 17: return "<device control 1>";
        case 18: return "<device control 2>";
        case 19: return "<device control 3>";
        case 20: return "<device control 4>";
        case 21: return "<negative acknowledge>";
        case 22: return "<synchronous idle>";
        case 23: return "<end of trans. block>";
        case 24: return "<cancel>";
        case 25: return "<end of medium>";
        case 26: return "<substitute>";
        case 27: return "<escape>";
        case 28: return "<file separator>";
        case 29: return "<group separator>";
        case 30: return "<record separator>";
        case 31: return "<unit separator>";
        case 127: return "<DEL>";
        case ' ': return "<space>";
        default: return fmt::to_string(c);
        }
    }();

    if (c == 0 && style == CharToStringStyle::smart)
    {
        return name;
    }

    constexpr std::string_view smart_characters =
        "abcdefghijklmnopqrstuwxyz"
        "ABCDEFGHIJKLMNOPQRSTUWXYZ"
        " "
        "~!@#$%^&*()_+"
        "`123456790-="
        ",.<>/?"
        "{}[]:;\"'\\|"
        "\n\r\t"
        ;

    if(style == CharToStringStyle::include_hex || smart_characters.find(c) == std::string_view::npos)
    {
        return fmt::format("{}({:#x})", name, c);
    }
    else
    {
        return name;
    }
}


std::string::size_type
find_first_index_of_mismatch(const std::string& lhs, const std::string& rhs)
{
    const auto end = std::min(lhs.size(), rhs.size());

    std::string::size_type index = 0;
    for(; index < end; index+=1)
    {
        if(lhs[index]!=rhs[index])
        {
            return index;
        }
    }

    if(index >= lhs.size() && index >= rhs.size())
    {
        return std::string::npos;
    }
    else
    {
        return end;
    }
}


void
replace_all(std::string* string, const std::string& to_find, const std::string& to_replace)
{
    std::size_t index = string->find(to_find);
    const std::size_t find_length = to_find.length();
    ASSERT(find_length > 0);
    while(index != std::string::npos)
    {
        string->erase(index, find_length);
        string->insert(index, to_replace);
        index = string->find(to_find, index);
    }
}


std::string
replace_all(const std::string& string, const std::string& to_find, const std::string& to_replace)
{
    std::string temp = string;
    replace_all(&temp, to_find, to_replace);
    return temp;
}


void
copy(char* dst, const std::string& src, const std::string::size_type& count)
{
    strncpy(dst, src.c_str(), count - 1);
    dst[count - 1] = 0;
}


std::string
replace_with_character(const std::string& string, const std::string& to_find, char to_replace)
{
    std::string s = string;
    for(char c: to_find)
    {
        std::replace(s.begin(), s.end(), c, to_replace);
    }
    return s;
}


std::string
remove_from_end(const std::string& str, const std::string& end)
{
    if(ends_with(str, end))
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
strip(const std::string& str, const std::string& ch)
{
    auto ss = StringBuilder{};
    for(const char c: str)
    {
        if(ch.find(c) == std::string::npos)
        {
            ss.add_char(c);
        }
    }
    return ss.to_string();
}


// remove all characters in ch except the first one in a chain from str
std::string
remove_consecutive(const std::string& str, const std::string& ch)
{
    auto ss = StringBuilder{};
    bool skip = false;
    for(const char c: str)
    {
        if(ch.find(c) == std::string::npos)
        {
            ss.add_char(c);
            skip = false;
        }
        else
        {
            if(!skip)
            {
                ss.add_char(c);
                skip = true;
            }
        }
    }
    return ss.to_string();
}


namespace
{
    enum class AddEmpty
    {
        no, yes
    };

    enum class AddEmptyLast
    {
        no, yes
    };

    template
    <
        typename IsDelimFunction
    >
    std::vector<std::string>
    split_base(const std::string& str, AddEmpty add_empty, AddEmptyLast add_empty_last, IsDelimFunction&& is_delim)
    {
        std::vector<std::string> ret;
        if (str.empty()) { return ret; }

        auto buffer = StringBuilder{};
        
        for(char c: str)
        {
            if (is_delim(c))
            {
                if (buffer.has_content())
                {
                    ret.emplace_back(buffer.to_string());
                    buffer.clear();
                }
                else if (add_empty == AddEmpty::yes)
                {
                    ret.emplace_back("");
                }
            }
            else
            {
                buffer.add_char(c);
            }
        }

        if (buffer.has_content())
        {
            ret.emplace_back(buffer.to_string());
            buffer.clear();
        }
        else if(add_empty_last == AddEmptyLast::yes)
        {
            ret.emplace_back("");
        }

        return ret;
    }
}


std::vector<std::string>
split(const std::string& s, char delim)
{
    return split_base
    (
        s, AddEmpty::yes, AddEmptyLast::yes, [&](char c)
        {
            return c == delim;
        }
    );
}


std::vector<std::string>
split_on_spaces(const std::string& string)
{
    return split_base
    (
        string, AddEmpty::no, AddEmptyLast::no, [](char c)
        {
            return space_characters.find(c) != std::string::npos;
        }
    );
}


std::string
get_string_or_empty(bool b, const std::string& str)
{
    if(b)
    {
        return str;
    }
    else
    {
        return "";
    }
}


bool
is_number(char b)
{
    return b >= '0' && b <= '9';
}


int
parse_number(const char** aa)
{
    const char*& a = *aa;

    int result = *a - '0';
    ++a;

    while(is_number(*a))
    {
        result *= 10;
        result += *a - '0';
        ++a;
    }

    --a;
    return result;
}


int
compare_string(const std::string& lhs, const std::string& rhs)
{
    const char* a = lhs.c_str();
    const char* b = rhs.c_str();

    if(a == b)    { return  0; }
    if(a == nullptr) { return -1; }
    if(b == nullptr) { return  1; }

    while(*a != 0 && *b != 0)
    {
        // will contain either a number or a letter
        const int a0 = is_number(*a) ? parse_number(&a) + 256 : to_lower_char(*a);
        const int b0 = is_number(*b) ? parse_number(&b) + 256 : to_lower_char(*b);

        if(a0 < b0) { return -1; }
        if(a0 > b0) { return  1; }

        ++a;
        ++b;
    }

    if(*a != 0) { return  1; }
    if(*b != 0) { return -1; }

    return 0;
}

}
