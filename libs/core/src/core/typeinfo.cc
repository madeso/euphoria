#include "typeinfo.h"

#if BUILD_TYPEINFO_IMPL_CUSTOM_RT == 1

#include <unordered_map>

namespace euphoria::core
{
    TypeId
    GetUniqueIdOf(const std::string& name)
    {
        static TypeId new_id = 0;
        static std::unordered_map<std::string, TypeId> saved;

        auto found = saved.find(name);
        if(found != saved.end())
        {
            return found->second;
        }

        const auto id = new_id;
        new_id += 1;
        saved[name] = id;
        return id;
    }

}

#endif
