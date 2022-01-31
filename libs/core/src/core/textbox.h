#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "assert/assert.h"

namespace euphoria::core
{
    // bitmasks
    constexpr unsigned char bit_up      = 1 << 0;
    constexpr unsigned char bit_down    = 1 << 1;
    constexpr unsigned char bit_left    = 1 << 2;
    constexpr unsigned char bit_right   = 1 << 3;
    constexpr unsigned char bit_no_line = static_cast<unsigned char>(~(bit_up | bit_down | bit_left | bit_right));

    struct TextBoxStyle
    {
    private:
        TextBoxStyle();

    public:
        static TextBoxStyle
        create(std::function<std::string(char)> connections_func);

        [[nodiscard]] auto
        get_string(char s) const -> std::string;

    private:
        std::vector<std::string> connections;
    };


    TextBoxStyle
    terminal_style();

    TextBoxStyle
    utf8_straight_style();

    TextBoxStyle
    utf8_rounded_style();

    TextBoxStyle
    utf_8double_line_style();

    TextBoxStyle
    ascii_style();

    /* TextBox: Abstraction for 2-dimensional text strings with linedrawing support
    Copyright (c) 2017 Joel Yliluoma - http://iki.fi/bisqwit/
    License: MIT
    https://gist.github.com/bisqwit/458048c60d271ab2536665cb81595c6b
    */
    // todo(Gustav): change to allow origin and negative drawing indices
    struct TextBox
    {
        static TextBox
        create_empty();

        static TextBox
        create_from_strings(const std::vector<std::string>& str);


        ///////////////////////////////////////////////////////////////////////////
        // Drawing functions

        /** Place a single character in the given coordinate.
        Notice that behavior is undefined if the character is in 00-1F range. */
        void
        put_char(int x, int y, char c);

        /** Modify a character using a callback */
        template<typename F>
        void
        mod_char(int x, int y, F&& func)
        {
            extend_to(x,y);
            func(data[y][x]);
        }

        /** Put a string of characters starting at the given coordinate.
        Note that behavior is undefined if the string contains characters in 00-1F range
        or if the string includes multibyte characters.
        */
        void
        put_string(int x, int y, const std::string& s);

        static TextBox
        from_string(const std::string& s, int x=0, int y=0);


        /* Put a 2D string starting at the given coordinate */
        void
        put_box(int x, int y, const TextBox& b);

        [[nodiscard]] TextBox
        put_box_copy(int x, int y, const TextBox& b) const;

        /** Draw a horizontal line.
        If bef=true, the line starts from the left edge of the first character cell, otherwise it starts from its center.
        If aft=true, the line ends in the right edge of the last character cell, otherwise it ends in its center */
        void
        put_horizontal_line
        (
            int x,
            int y,
            int width,
            bool bef,
            bool aft
        );

        /** Draw a vertical line.
        If bef=true, the line starts from the top edge of the first character cell, otherwise it starts from its center.
        If aft=true, the line ends in the bottom edge of the last character cell, otherwise it ends in its center */
        void
        put_vertical_line
        (
            int x,
            int y,
            int height,
            bool bef,
            bool aft
        );


        ///////////////////////////////////////////////////////////////////////////
        // General operations

        [[nodiscard]] std::vector<std::string>
        to_string(const TextBoxStyle& style = terminal_style()) const;

        /* Delete trailing blank from the bottom and right edges */
        void
        trim();


        ///////////////////////////////////////////////////////////////////////////
        // Status

        /* Calculate the current dimensions of the string */
        [[nodiscard]] int
        get_height() const;

        [[nodiscard]] int
        get_width() const;

        // width x height
        [[nodiscard]] std::pair<int, int>
        get_size() const;

        ///////////////////////////////////////////////////////////////////////////
        // Tree graph

        /* An utility function that can be used to create a tree graph rendering from a structure.
        *
        * Parameters:
        *   e:                The element that will be rendered.
        *                     Possibly some user-defined type that represents a node in a tree structure.
        *   maxwidth:         The maximum width of the resulting box in characters.
        *   to_string:         A functor of type std::string(const T&).
        *                     It renders the given element into an 1D string.
        *                     Note that the string must not contain multibyte characters,
        *                     because size() will be used to determine its width in columns.
        *   count_children:   A functor of type std::pair<ForwardIterator,ForwardIterator>(const T&).
        *                     It returns a pair of iterators representing the range of children
        *                     for the given element.
        *                     create_tree_graph will call itself recursively for each element in this range.
        *   oneliner_test:    A functor of type bool(const T&).
        *                     If the result is true, enables simplified horizontal topology.
        *   simple_test:      A functor of type bool(const T&).
        *                     If the result is true, enables very simplified horizontal topology.
        *   separate1st_test: A functor of type bool(const T&).
        *                     If the result is true, create_tree_graph() will always render
        *                     the first child alone on a separate line, but the rest of them
        *                     may get rendered horizontally.
        *
        * @param margin the spacing between children
        * @param firstx the first child offset
        *
        * Topology types:
        *
        *        Vertical:
        *
        *              element
        *              ├─child1
        *              ├─child2
        *              └─child3
        *
        *        Horizontal:
        *
        *              element
        *              └─┬─────────┬─────────┐
        *                child1    child2    child3
        *
        *        Simplified horizontal:
        *
        *              element──┬───────┬───────┐
        *                       child1  child2  child3
        *
        *        Very simplified horizontal:
        *
        *              element──child1
        *
        * The vertical and horizontal topologies are automatically chosen
        * depending on the situation compared to the maxwidth parameter,
        * and according to the constraint given by separate1st_test().
        *
        * Simplified topology will be used if oneliner_test() returns true,
        *                                     separate1st_test() returns false,
        *                                     all children fit on one line,
        *                                     and very simplified topology is not used.
        *
        * Very simplified topology will be used if oneliner_test() returns true,
        *                                          separate1st_test() returns false,
        *                                          simple_test() returns true,
        *                                          there is only 1 child,
        *                                          and it fits on one line.
        */
        template
        <
            typename T,
            typename ToStringFunction,
            typename ParamCountFunc,
            typename OneLinerFunc,
            typename SimpleTestFunc
        >
        static TextBox
        create_tree_graph
        (
            const T& e,
            int maxwidth,
            ToStringFunction&& to_string,
            ParamCountFunc&& count_children,
            OneLinerFunc&& oneliner_test,
            SimpleTestFunc&& simple_test,
            int margin = 4,
            int firstx = 2
        )
        {
            ASSERTX(maxwidth >=16, maxwidth);

            const auto label = to_string(e);
            auto result = TextBox::from_string(label);

            if(auto [begin, end] = count_children(e); begin != end)
            {
                std::vector<TextBox> boxes;
                boxes.reserve(std::distance(begin, end));
                for(auto i = begin; i != end; ++i)
                {
                    boxes.emplace_back
                    (
                        create_tree_graph
                        (
                            *i,
                            std::max<int>(maxwidth - 2, 16),
                            to_string,
                            count_children,
                            oneliner_test,
                            simple_test,
                            margin,
                            firstx
                        )
                    );
                }
                sub_create_tree_graph
                (
                    &result,
                    maxwidth,
                    boxes,
                    oneliner_test(e),
                    simple_test(e),
                    label,
                    margin,
                    firstx
                );
            }
            result.trim();
            return result;
        }


        ///////////////////////////////////////////////////////////////////////////
        // private functions

        void
        extend_to(int x, int y);

        /** Calculate the earliest X coordinate where the given box could be placed.
        without colliding with existing content in this box. Guaranteed to be <= width().
        Find leftmost position where box b can be appended into *this without overlap
        */
        [[nodiscard]] int
        get_horizontal_append_position(int y, const TextBox& b) const;

        /** Calculate the earliest Y coordinate where the given box could be placed without colliding with existing content in this box. Guaranteed to be <= height().
        * Find topmost position where box b can be appended into *this without overlap
        */
        [[nodiscard]] int
        get_vertical_append_position(int x, const TextBox& b) const;

        [[nodiscard]] int
        find_left_padding(int y) const;

        [[nodiscard]] int
        find_right_padding(int y) const;

        [[nodiscard]] int
        find_top_padding(int x) const;

        [[nodiscard]] int
        find_bottom_padding(int x) const;

    private:
        static void
        sub_create_tree_graph
        (
            TextBox* result,
            int maxwidth,
            const std::vector<TextBox>& boxes,
            bool oneliner_test,
            bool simple_test,
            const std::string& label,
            int margin,
            int firstx
        );

        std::vector<std::string> data;
        TextBox();
    };
}
