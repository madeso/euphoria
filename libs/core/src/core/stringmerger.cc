#include "core/stringmerger.h"

#include <string_view>

#include "core/str.h"
#include "core/stringbuilder.h"


namespace euphoria::core
{
    std::string
    StringMerger::merge(const std::vector<std::string>& strings) const
    {
        if(strings.empty())
        {
            return "{}{}{}"_format(start, empty, end);
        }

        auto ss = StringBuilder2{};

        ss.add_view(start);

        const auto count = strings.size();
        for(std::vector<std::string>::size_type index = 0; index < count; ++index)
        {
            ss.add_view(before_each);
            ss.add_string(strings[index]);

            if(index == count - 1)
            {
                ss.add_view(final_after_each);
            }
            else
            {
                ss.add_view(after_each);
            }

            if(count != index + 1)
            { // if this item isn't the last one in the list
                if(count == index + 2)
                {
                    ss.add_view(final_separator);
                }
                else
                {
                    ss.add_view(separator);
                }
            }
        }

        ss.add_view(end);

        return ss.to_string();
    }
}
