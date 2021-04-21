#include "core/ctree.h"

#include <cstdlib>

#include "core/stringutils.h"

namespace
{
    bool TerminalSupportUtf8()
    {
        const auto* clang = std::getenv("LANG");

        if(clang != nullptr)
        {
            const auto lang = std::string(clang);
            const auto lower = euphoria::core::ToLower(lang);
            const auto ends = euphoria::core::EndsWith(lower, "utf-8");
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
    if(TerminalSupportUtf8())
    {
        return utf8_tree_style();
    }
    else
    {
        return cross_style();
    }
}

}
