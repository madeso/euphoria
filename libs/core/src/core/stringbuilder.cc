#include "assert/assert.h"

#include "core/stringbuilder.h"

namespace euphoria::core
{


namespace
{
    bool can_add_to(const std::vector<char>& data)
    {
        // empty strings are ok
        if(data.empty()) { return true; }

        // string is valid if it doesn't end with a null terminator
        return *data.rbegin() != '\0';
    }
}


bool StringBuilder2::has_content() const
{
    return data.empty() == false;
}


void StringBuilder2::clear()
{
    data.clear();
}


void StringBuilder2::add_char(char c)
{
    ASSERT(can_add_to(data));
    data.emplace_back(c);
}


void StringBuilder2::add_string(const std::string& str)
{
    ASSERT(can_add_to(data));
    for(char c: str)
    {
        data.emplace_back(c);
    }
}


void StringBuilder2::add_view(const std::string_view& str)
{
    ASSERT(can_add_to(data));
    for(char c: str)
    {
        data.emplace_back(c);
    }
}


std::string StringBuilder2::to_string()
{
    ASSERT(can_add_to(data));
    data.emplace_back('\0'); // now we no longer can add to it!
    return data.data();
}

}

