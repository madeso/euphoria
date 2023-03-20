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

// todo(Gustav): make constexpr

TreeStyle slash_style()
{
    auto r = TreeStyle{};
    r.t_cross = "|-";
    r.l = "\\-";
    r.down = "| ";
    r.space = "  ";
    r.include_space = true;
    r.root_special = true;
    return r;
}

TreeStyle utf8_tree_style()
{
    auto r = TreeStyle{};
    r.t_cross = "├╴";
    r.l = "└╴";
    r.down = "│ ";
    r.space = "  ";
    r.include_space = true;
    r.root_special = true;
    return r;
}

TreeStyle cross_style()
{
    auto r = TreeStyle{};
    r.t_cross = "+- ";
    r.l = "+- ";
    r.down = "|  ";
    r.space = "   ";
    r.include_space = false;
    r.root_special = false;
    return r;
}

TreeStyle determine_style()
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
