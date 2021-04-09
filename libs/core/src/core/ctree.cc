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

TreeStyle SlashStyle()
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

TreeStyle Utf8TreeStyle()
{
    auto r = TreeStyle{};
    r.t_cross = u8"├╴";
    r.l = u8"└╴";
    r.down = u8"│ ";
    r.space = u8"  ";
    r.include_space = true;
    r.root_special = true;
    return r;
}

TreeStyle CrossStyle()
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

TreeStyle DetermineStyle()
{
    if(TerminalSupportUtf8())
    {
        return Utf8TreeStyle();
    }
    else
    {
        return CrossStyle();
    }
}

}
