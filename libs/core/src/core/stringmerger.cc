#include "core/stringmerger.h"

#include "core/str.h"

#include <string_view>
#include <sstream>
#include <vector>

namespace euphoria::core
{
    std::string
    StringMerger::merge(const std::vector<std::string>& strings) const
    {
        if(strings.empty())
        {
            return StringBuilder() << start << empty << end;
        }

        std::ostringstream ss;

        ss << start;

        const auto count = strings.size();
        for(std::vector<std::string>::size_type index = 0; index < count; ++index)
        {
            ss << before_each << strings[index];

            if(index == count - 1)
            {
                ss << final_after_each;
            }
            else
            {
                ss << after_each;
            }

            if(count != index + 1)
            { // if this item isn't the last one in the list
                if(count == index + 2)
                {
                    ss << final_separator;
                }
                else
                {
                    ss << separator;
                }
            }
        }

        ss << end;

        return ss.str();
    }
}
