#pragma once


#include <array>
#include <algorithm>
#include <functional>

#include "assert/assert.h"

namespace euphoria::core
{
    // bitmasks
    constexpr unsigned char bit_up = 1 << 0;
    constexpr unsigned char bit_down = 1 << 1;
    constexpr unsigned char bit_left = 1 << 2;
    constexpr unsigned char bit_right = 1 << 3;
    constexpr unsigned char bit_no_line = static_cast<unsigned char>(~(bit_up | bit_down | bit_left | bit_right));

    struct TextBoxStyle
    {
        template<typename F>
        constexpr explicit TextBoxStyle(F&& connections_func)
        {
            for (char c = 0; c < 15; c += 1)
            {
                connections[c] = connections_func(static_cast<char>(c + 1));
            }
        }

        [[nodiscard]] std::string_view get_string(char s) const;

    private:
        std::array<std::string_view, 15> connections;

        constexpr TextBoxStyle() = default;
    };

    TextBoxStyle get_terminal_style();

    constexpr TextBoxStyle utf8_straight_style = TextBoxStyle{ [](char c) {
        switch (c)
        {
        case bit_left:
        case bit_right:
        case bit_left | bit_right:                     return "─";
        case bit_up:
        case bit_down:
        case bit_up | bit_down:                        return "│";
        case bit_left | bit_up:                         return "┘";
        case bit_left | bit_down:                      return "┐";
        case bit_right | bit_up:                       return "└";
        case bit_right | bit_down:                     return "┌";
        case bit_left | bit_right | bit_up:            return "┴";
        case bit_left | bit_right | bit_down:          return "┬";
        case bit_left | bit_up | bit_down:             return "┤";
        case bit_right | bit_up | bit_down:            return "├";
        case bit_left | bit_right | bit_up | bit_down: return "┼";
        default:
            DIE("Invalid combination");
            return "X";
        }
    }};

    constexpr TextBoxStyle utf8_rounded_style = TextBoxStyle{ [](char c) {
        switch (c)
        {
        case bit_left:
        case bit_right:
        case bit_left | bit_right:                     return "─";
        case bit_up:
        case bit_down:
        case bit_up | bit_down:                        return "│";
        case bit_left | bit_up:                         return "╯";
        case bit_left | bit_down:                      return "╮";
        case bit_right | bit_up:                       return "╰";
        case bit_right | bit_down:                     return "╭";
        case bit_left | bit_right | bit_up:            return "┴";
        case bit_left | bit_right | bit_down:          return "┬";
        case bit_left | bit_up | bit_down:             return "┤";
        case bit_right | bit_up | bit_down:            return "├";
        case bit_left | bit_right | bit_up | bit_down: return "┼";
        default:
            DIE("Invalid combination");
            return "X";
        }
    }};

    constexpr TextBoxStyle utf_8double_line_style = TextBoxStyle{ [](char c) {
        switch (c)
        {
        case bit_left:
        case bit_right:
        case bit_left | bit_right:                     return "═";
        case bit_up:
        case bit_down:
        case bit_up | bit_down:                        return "║";
        case bit_left | bit_up:                         return "╝";
        case bit_left | bit_down:                      return "╗";
        case bit_right | bit_up:                       return "╚";
        case bit_right | bit_down:                     return "╔";
        case bit_left | bit_right | bit_up:            return "╩";
        case bit_left | bit_right | bit_down:          return "╦";
        case bit_left | bit_up | bit_down:             return "╣";
        case bit_right | bit_up | bit_down:            return "╠";
        case bit_left | bit_right | bit_up | bit_down: return "╬";
        default:
            DIE("Invalid combination");
            return "X";
        }
    }};

    constexpr TextBoxStyle ascii_style = TextBoxStyle { [](char c) {
        switch (c)
        {
        case bit_left:
        case bit_right:
        case bit_left | bit_right:                     return "-";
        case bit_up:
        case bit_down:
        case bit_up | bit_down:                        return "|";
        case bit_left | bit_up:                         return "'";
        case bit_left | bit_down:                      return ".";
        case bit_right | bit_up:                       return "`";
        case bit_right | bit_down:                     return ",";
        case bit_left | bit_right | bit_up:
        case bit_left | bit_right | bit_down:
        case bit_left | bit_up | bit_down:
        case bit_right | bit_up | bit_down:
        case bit_left | bit_right | bit_up | bit_down: return "+";
        default:
            DIE("Invalid combination");
            return "X";
        }
    }};

    /* TextBox: Abstraction for 2-dimensional text strings with linedrawing support
    Copyright (c) 2017 Joel Yliluoma - http://iki.fi/bisqwit/
    License: MIT
    https://gist.github.com/bisqwit/458048c60d271ab2536665cb81595c6b
    */
    // todo(Gustav): change to allow origin and negative drawing indices
    struct TextBox
    {
        static TextBox create_empty();
        static TextBox create_from_strings(const std::vector<std::string>& str);
        static TextBox from_string(const std::string& s, int x = 0, int y = 0);

        /** An utility function that can be used to create a tree graph rendering from a structure.
        *
        *   @param e          The element that will be rendered.
        *                     Possibly some user-defined type that represents a node in a tree structure.
        *   @param maxwidth   The maximum width of the resulting box in characters.
        *   @param to_string  A functor of type std::string(const T&).
        *                     It renders the given element into an 1D string.
        *                     Note that the string must not contain multibyte characters,
        *                     because size() will be used to determine its width in columns.
        * @param count_children_func A functor of type std::pair<ForwardIterator,ForwardIterator>(const T&).
        *                     It returns a pair of iterators representing the range of children
        *                     for the given element.
        *                     create_tree_graph will call itself recursively for each element in this range.
        * @param oneliner_test_func A functor of type bool(const T&).
        *                      If the result is true, enables simplified horizontal topology.
        * @param simple_test_func  A functor of type bool(const T&).
        *                     If the result is true, enables very simplified horizontal topology.
        *
        * @param margin the spacing between children
        * @param firstx the first child offset
        *
        * Topology types:
        *
        *        Vertical:
        *```
        *              element
        *              ├─child1
        *              ├─child2
        *              └─child3
        * ```
        *        Horizontal:
        * ```
        *              element
        *              └─┬─────────┬─────────┐
        *                child1    child2    child3
        * ```
        *        Simplified horizontal:
        * ```
        *              element──┬───────┬───────┐
        *                       child1  child2  child3
        * ```
        * 
        * The vertical and horizontal topologies are automatically chosen
        * depending on the situation compared to the maxwidth parameter.
        *
        * Simplified topology will be used if
        *   * oneliner_test_func() returns true,
        *   * all children fit on one line,
        *   * and very simplified topology is not used.
        *
        * Very simplified topology will be used if
        *   * oneliner_test_func() returns true,
        *   * simple_test_func() returns true,
        *   * there is only 1 child,
        *   * and it fits on one line.
        */
        template
        <
            typename T,
            typename TToStringFunc,
            typename TCountChildrenFunc,
            typename TOneLinerTestFunc,
            typename TSimpleTestFunc
        >
        static TextBox
        create_tree_graph
        (
            const T& e,
            int maxwidth,
            TToStringFunc&& to_string,
            TCountChildrenFunc&& count_children_func,
            TOneLinerTestFunc&& oneliner_test_func,
            TSimpleTestFunc&& simple_test_func,
            int margin = 4,
            int firstx = 2
        )
        {
            ASSERTX(maxwidth >=16, maxwidth);

            const auto label = to_string(e);
            auto result = TextBox::from_string(label);

            if(auto [begin, end] = count_children_func(e); begin != end)
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
                            count_children_func,
                            oneliner_test_func,
                            simple_test_func,
                            margin,
                            firstx
                        )
                    );
                }
                create_tree_graph_impl
                (
                    &result,
                    maxwidth,
                    boxes,
                    oneliner_test_func(e),
                    simple_test_func(e),
                    label,
                    margin,
                    firstx
                );
            }
            result.trim();
            return result;
        }

        ///////////////////////////////////////////////////////////////////////////
        // Drawing functions

        /** Place a single character in the given coordinate.
        Notice that behavior is undefined if the character is in 00-1F range. */
        void put_char(int x, int y, char c);

        /** Modify a character using a callback */
        template<typename F>
        void mod_char(int x, int y, F&& func)
        {
            extend_to(x,y);
            func(data[y][x]);
        }

        /** Put a string of characters starting at the given coordinate.
        Note that behavior is undefined if the string contains characters in 00-1F range
        or if the string includes multibyte characters.
        */
        void put_string(int x, int y, const std::string& s);

        /* Put a 2D string starting at the given coordinate */
        void put_box(int x, int y, const TextBox& b);

        /** Draw a horizontal line.
        If bef=true, the line starts from the left edge of the first character cell, otherwise it starts from its center.
        If aft=true, the line ends in the right edge of the last character cell, otherwise it ends in its center */
        void put_horizontal_line
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
        void put_vertical_line
        (
            int x,
            int y,
            int height,
            bool bef,
            bool aft
        );

        /// Delete trailing blank from the bottom and right edges
        void trim();



        [[nodiscard]] TextBox put_box_copy(int x, int y, const TextBox& b) const;
        [[nodiscard]] std::vector<std::string> to_string(const TextBoxStyle& style = get_terminal_style()) const;

        /** Calculate the earliest X coordinate where the given box could be placed.
        without colliding with existing content in this box. Guaranteed to be <= width().
        Find leftmost position where box b can be appended into *this without overlap
        */
        [[nodiscard]] int get_horizontal_append_position(int y, const TextBox& b) const;

        /** Calculate the earliest Y coordinate where the given box could be placed without colliding with existing content in this box. Guaranteed to be <= height().
        * Find topmost position where box b can be appended into *this without overlap
        */
        [[nodiscard]] int get_vertical_append_position(int x, const TextBox& b) const;

        /// Calculate the current dimensions of the string
        [[nodiscard]] int get_height() const;

        [[nodiscard]] int get_width() const;

        /// @return width x height
        [[nodiscard]] std::pair<int, int> get_size() const;

    private:
        std::vector<std::string> data;
        
        TextBox();

        void extend_to(int x, int y);

        [[nodiscard]] int find_left_padding(int y) const;
        [[nodiscard]] int find_right_padding(int y) const;
        [[nodiscard]] int find_top_padding(int x) const;
        [[nodiscard]] int find_bottom_padding(int x) const;

        static void create_tree_graph_impl
        (
            TextBox* result,
            int maxwidth,
            const std::vector<TextBox>& boxes,
            bool oneliner_test_func,
            bool simple_test_func,
            const std::string& label,
            int margin,
            int firstx
        );
    };
}
