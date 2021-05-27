#ifndef EUPHORIA_CORE_TEXTBOX_H
#define EUPHORIA_CORE_TEXTBOX_H

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "core/assert.h"

namespace euphoria::core
{

    // bitmasks
    constexpr unsigned char bit_up      = 1 << 0;
    constexpr unsigned char bit_down    = 1 << 1;
    constexpr unsigned char bit_left    = 1 << 2;
    constexpr unsigned char bit_right   = 1 << 3;
    constexpr unsigned char bit_no_line = ~(bit_up | bit_down | bit_left | bit_right);

    struct text_box_style
    {
    private:
        text_box_style();

    public:
        static text_box_style
        create(std::function<std::string(char)> connections_func);

        [[nodiscard]] auto
        get_string(char s) const -> std::string;

    private:
        std::vector<std::string> connections;
    };


    text_box_style
    terminal_style();

    text_box_style
    utf8_straight_style();

    text_box_style
    utf8_rounded_style();

    text_box_style
    utf_8double_line_style();

    text_box_style
    ascii_style();

    /* TextBox: Abstraction for 2-dimensional text strings with linedrawing support
    Copyright (c) 2017 Joel Yliluoma - http://iki.fi/bisqwit/
    License: MIT
    https://gist.github.com/bisqwit/458048c60d271ab2536665cb81595c6b
    */
    // todo(Gustav): change to allow origin and negative drawing indices
    struct text_box
    {
        static text_box
        create_empty();

        static text_box
        create_from_strings(const std::vector<std::string>& str);


        ///////////////////////////////////////////////////////////////////////////
        // Drawing functions

        /** Place a single character in the given coordinate.
        Notice that behavior is undefined if the character is in 00-1F range. */
        void
        put_char(std::size_t x, std::size_t y, char c);

        /** Modify a character using a callback */
        template<typename F>
        void
        mod_char(std::size_t x, std::size_t y, F&& func)
        {
            extend_to(x,y);
            func(data[y][x]);
        }

        /** Put a string of characters starting at the given coordinate.
        Note that behavior is undefined if the string contains characters in 00-1F range
        or if the string includes multibyte characters.
        */
        void
        put_string(std::size_t x, std::size_t y, const std::string& s);

        static text_box
        from_string(const std::string& s, std::size_t x=0, std::size_t y=0);


        /* Put a 2D string starting at the given coordinate */
        void
        put_box(std::size_t x, std::size_t y, const text_box& b);

        [[nodiscard]] text_box
        put_box_copy(std::size_t x, std::size_t y, const text_box& b) const;

        /** Draw a horizontal line.
        If bef=true, the line starts from the left edge of the first character cell, otherwise it starts from its center.
        If aft=true, the line ends in the right edge of the last character cell, otherwise it ends in its center */
        void
        put_horizontal_line
        (
            std::size_t x,
            std::size_t y,
            std::size_t width,
            bool bef,
            bool aft
        );

        /** Draw a vertical line.
        If bef=true, the line starts from the top edge of the first character cell, otherwise it starts from its center.
        If aft=true, the line ends in the bottom edge of the last character cell, otherwise it ends in its center */
        void
        put_vertical_line
        (
            std::size_t x,
            std::size_t y,
            std::size_t height,
            bool bef,
            bool aft
        );


        ///////////////////////////////////////////////////////////////////////////
        // General operations

        [[nodiscard]] std::vector<std::string>
        to_string(const text_box_style& style = terminal_style()) const;

        /* Delete trailing blank from the bottom and right edges */
        void
        trim();


        ///////////////////////////////////////////////////////////////////////////
        // Status

        /* Calculate the current dimensions of the string */
        [[nodiscard]] std::size_t
        get_height() const;

        [[nodiscard]] std::size_t
        get_width() const;

        // width x height
        [[nodiscard]] std::pair<std::size_t, std::size_t>
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
        static text_box
        create_tree_graph
        (
            const T& e,
            std::size_t maxwidth,
            ToStringFunction&& to_string,
            ParamCountFunc&& count_children,
            OneLinerFunc&& oneliner_test,
            SimpleTestFunc&& simple_test,
            std::size_t margin = 4,
            std::size_t firstx = 2
        )
        {
            ASSERTX(maxwidth >=16, maxwidth);

            const auto label = to_string(e);
            auto result = text_box::from_string(label);

            if(auto [begin, end] = count_children(e); begin != end)
            {
                std::vector<text_box> boxes;
                boxes.reserve(std::distance(begin, end));
                for(auto i = begin; i != end; ++i)
                {
                    boxes.emplace_back
                    (
                        create_tree_graph
                        (
                            *i,
                            std::max<std::size_t>(maxwidth - 2, 16),
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
        extend_to(std::size_t x, std::size_t y);

        /** Calculate the earliest X coordinate where the given box could be placed.
        without colliding with existing content in this box. Guaranteed to be <= width().
        Find leftmost position where box b can be appended into *this without overlap
        */
        [[nodiscard]] std::size_t
        get_horizontal_append_position(std::size_t y, const text_box& b) const;

        /** Calculate the earliest Y coordinate where the given box could be placed without colliding with existing content in this box. Guaranteed to be <= height().
        * Find topmost position where box b can be appended into *this without overlap
        */
        [[nodiscard]] std::size_t
        get_vertical_append_position(std::size_t x, const text_box& b) const;

        [[nodiscard]] std::size_t
        find_left_padding(std::size_t y) const;

        [[nodiscard]] std::size_t
        find_right_padding(std::size_t y) const;

        [[nodiscard]] std::size_t
        find_top_padding(std::size_t x) const;

        [[nodiscard]] std::size_t
        find_bottom_padding(std::size_t x) const;

    private:
        static void
        sub_create_tree_graph
        (
            text_box* result,
            size_t maxwidth,
            const std::vector<text_box>& boxes,
            bool oneliner_test,
            bool simple_test,
            const std::string& label,
            std::size_t margin,
            std::size_t firstx
        );

        std::vector<std::string> data;
        text_box();
    };
}

#endif // EUPHORIA_CORE_TEXTBOX_H

