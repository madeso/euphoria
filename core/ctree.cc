#include "core/ctree.h"

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

TreeStyle Utf8Style()
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
    return CrossStyle();
}

}
