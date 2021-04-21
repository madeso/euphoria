#ifndef EUPHORIA_CORE_CTREE_H
#define EUPHORIA_CORE_CTREE_H

#include <string>

namespace euphoria::core
{
    struct tree_style
    {
        std::string_view t_cross;
        std::string_view l;
        std::string_view down;
        std::string_view space;

        bool include_space = false;
        bool root_special = true;
    };


    tree_style
    slash_style();
    

    tree_style
    cross_style();
    

    tree_style
    utf8_tree_style();


    tree_style
    determine_style();


    template
    <
        typename T,
        typename LabelFunction,
        typename FindChildrenFunction,
        typename PrintFunction
    >
    void
    print_hierarchy
    (
        T item,
        LabelFunction label_function,
        FindChildrenFunction find_children_function,
        PrintFunction print_function,
        const tree_style& style = determine_style(),
        const std::string& a_indent = "",
        int index = 0,
        bool last = true
    )
    {
        const auto children = find_children_function(item);
        const auto label = label_function(item);
        const auto is_root = style.root_special && index  == 0;

        std::string indent = a_indent;

        if (is_root)
        {
            print_function(label);
        }
        else
        {
            print_function(indent + std::string{ last ? style.l : style.t_cross } +label);

            if ( style.include_space && last && children.empty())
            {
                print_function(indent);
            }

            indent += (last ? style.space : style.down);
        }

        for (auto it = children.begin(); it != children.end(); ++it)
        {
            print_hierarchy<T, LabelFunction, FindChildrenFunction, PrintFunction>
            (
                *it,
                label_function,
                find_children_function,
                print_function,
                style,
                indent,
                index + 1,
                it + 1 == children.end()
            );
        }
    }
}


#endif // EUPHORIA_CORE_CTREE_H
