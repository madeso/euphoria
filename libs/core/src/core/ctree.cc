#include "core/ctree.h"

#include <cstdlib>

#include "base/stringutils.h"

namespace
{
    bool is_terminal_supporting_utf8()
    {
        const auto* clang = std::getenv("LANG");

        if(clang != nullptr)
        {
            const auto lang = std::string(clang);
            const auto lower = eu::to_lower(lang);
            const auto ends = eu::ends_with(lower, "utf-8");
            return ends;
        }

        return false;
    }
}

namespace eu::core
{


TreeStyle determine_style()
{
    if(is_terminal_supporting_utf8())
    {
        return utf8_tree_style;
    }
    else
    {
        return cross_style;
    }
}

}
