#pragma once



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

    // todo(Gustav): make constexpr
    TreeStyle
    slash_style();


    TreeStyle
    cross_style();


    TreeStyle
    utf8_tree_style();


    TreeStyle
    determine_style();


    template
    <
        typename T,
        typename TLabelFunc,
        typename TFindChildrenFunc,
        typename TPrintFunc
    >
    void
    print_hierarchy
    (
        T item,
        TLabelFunc label_func,
        TFindChildrenFunc find_children_func,
        TPrintFunc print_func,
        const TreeStyle& style = determine_style(),
        const std::string& a_indent = "",
        int index = 0,
        bool last = true
    )
    {
        const auto children = find_children_func(item);
        const auto label = label_func(item);
        const auto is_root = style.root_special && index == 0;

        std::string indent = a_indent;

        if (is_root)
        {
            print_func(label);
        }
        else
        {
            print_func(indent + std::string{ last ? style.l : style.t_cross } +label);

            if ( style.include_space && last && children.empty())
            {
                print_func(indent);
            }

            indent += (last ? style.space : style.down);
        }

        for (auto it = children.begin(); it != children.end(); ++it)
        {
            print_hierarchy<T, TLabelFunc, TFindChildrenFunc, TPrintFunc>
            (
                *it,
                label_func,
                find_children_func,
                print_func,
                style,
                indent,
                index + 1,
                it + 1 == children.end()
            );
        }
    }
}
