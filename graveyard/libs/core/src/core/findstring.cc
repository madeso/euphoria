#include "core/findstring.h"

#include <algorithm>

namespace eu::core
{
    bool
    find(const std::string& target, const std::string& search)
    {
        return target.find(search) != std::string::npos;
    }


    bool
    find(const std::string& target, const std::vector<std::string>& searches)
    {
        return std::all_of
        (
            searches.begin(),
            searches.end(),
            [&target](const auto& search)
            {
                return find(target, search);
            }
        );
    }


    bool
    find(const std::vector<std::string>& targets, const std::string& search)
    {
        return std::any_of
        (
            targets.begin(),
            targets.end(),
            [&search](const auto& target)
            {
                return find(target, search);
            }
        );
    }


    bool
    find(const std::vector<std::string>& targets,
         const std::vector<std::string>& searches)
    {
        return std::any_of
        (
            targets.begin(),
            targets.end(),
            [&searches](const auto& target)
            {
                return find(target, searches);
            }
        );
    }

}
