#ifndef EUPHORIA_CORE_CTREE_H
#define EUPHORIA_CORE_CTREE_H

#include <string>

namespace euphoria::core
{
    struct TreeStyle
    {
        std::string_view t_cross;
        std::string_view l;
        std::string_view down;
        std::string_view space;

        bool include_space = false;
        bool root_special = true;
    };


    TreeStyle
    SlashStyle();
    

    TreeStyle
    CrossStyle();
    

    TreeStyle
    Utf8TreeStyle();


    TreeStyle
    DetermineStyle();


    template
    <
        typename T,
        typename LabelFunction,
        typename FindChildrenFunction,
        typename PrintFunction
    >
    void
    PrintHierarchy
    (
        T item,
        LabelFunction label,
        FindChildrenFunction findChildren,
        PrintFunction print,
        const TreeStyle& style = DetermineStyle(),
        const std::string& a_indent = "",
        int index = 0,
        bool last = true
    )
    {
        const auto kids = findChildren(item);
        const auto labl = label(item);
        const auto root = style.root_special && index  == 0;

        std::string indent = a_indent;

        if (root)
        {
            print(labl);
        }
        else
        {
            print(indent + std::string{ last ? style.l : style.t_cross } +labl);

            if ( style.include_space && last && kids.empty())
            {
                print(indent);
            }

            indent += (last ? style.space : style.down);
        }

        for (auto it = kids.begin(); it != kids.end(); ++it)
        {
            PrintHierarchy<T, LabelFunction, FindChildrenFunction, PrintFunction>
            (
                *it,
                label,
                findChildren,
                print,
                style,
                indent,
                index + 1,
                it + 1 == kids.end()
            );
        }
    }
}


#endif // EUPHORIA_CORE_CTREE_H
