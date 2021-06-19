#include "core/ctree.h"

#include <cstdlib>

#include "core/stringutils.h"

namespace
{
    bool is_terminal_supporting_utf8()
    {
        const auto* clang = std::getenv("LANG");

        if(clang != nullptr)
        {
            const auto lang = std::string(clang);
            const auto lower = euphoria::core::to_lower(lang);
            const auto ends = euphoria::core::ends_with(lower, "utf-8");
            return ends;
        }

        return false;
    }
}

namespace euphoria::core
{

tree_style slash_style()
{
    auto r = tree_style{};
    r.t_cross = "|-";
    r.l = "\\-";
    r.down = "| ";
    r.space = "  ";
    r.include_space = true;
    r.root_special = true;
    return r;
}

tree_style utf8_tree_style()
{
    auto r = tree_style{};
    r.t_cross = u8"├╴";
    r.l = u8"└╴";
    r.down = u8"│ ";
    r.space = u8"  ";
    r.include_space = true;
    r.root_special = true;
    return r;
}

tree_style cross_style()
{
    auto r = tree_style{};
    r.t_cross = "+- ";
    r.l = "+- ";
    r.down = "|  ";
    r.space = "   ";
    r.include_space = false;
    r.root_special = false;
    return r;
}

tree_style determine_style()
{
    if(is_terminal_supporting_utf8())
    {
        return utf8_tree_style();
    }
    else
    {
        return cross_style();
    }
}

}
