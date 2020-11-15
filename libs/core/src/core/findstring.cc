#include "core/findstring.h"

namespace euphoria::core
{
    bool
    Find(const std::string& target, const std::string& search)
    {
        return target.find(search) != std::string::npos;
    }

    bool
    Find(const std::string& target, const std::vector<std::string>& searches)
    {
        for(const auto& search: searches)
        {
            if(!Find(target, search))
            {
                return false;
            }
        }

        return true;
    }

    bool
    Find(const std::vector<std::string>& targets, const std::string& search)
    {
        for(const auto& target: targets)
        {
            if(Find(target, search))
            {
                return true;
            }
        }

        return false;
    }

    bool
    Find(const std::vector<std::string>& targets,
         const std::vector<std::string>& searches)
    {
        for(const auto& target: targets)
        {
            if(Find(target, searches))
            {
                return true;
            }
        }

        return false;
    }

}  // namespace euphoria::core
