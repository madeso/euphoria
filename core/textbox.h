#ifndef EUPHORIA_CORE_TEXTBOX_H
#define EUPHORIA_CORE_TEXTBOX_H

#include <string>
#include <vector>
#include <algorithm>

namespace euphoria::core
{

    struct TextBoxStyle
    {
        TextBoxStyle();
        std::vector<std::string> connections;
    };

    TextBoxStyle TerminalStyle();

    TextBoxStyle Utf8Style();
    TextBoxStyle AsciiStyle();

/* TextBox: Abstraction for 2-dimensional text strings, with VT100 linedrawing support
Copyright (c) 2017 Joel Yliluoma - http://iki.fi/bisqwit/
License: MIT
Requires a C++17 capable compiler and standard library. */
struct TextBox
{
    std::vector<std::string> data;

    TextBox();
    TextBox(const std::vector<std::string>& a_data);

    /** Place a single character in the given coordinate.
    Notice that behavior is undefined if the character is in 00-1F range. */
    void putchar(char c, std::size_t x, std::size_t y);


    void extend(std::size_t x, std::size_t y);

    /** Modify a character using a callback */
    template<typename F>
    void modchar(std::size_t x, std::size_t y, F&& func)
    {
        extend(x,y);
        func(data[y][x]);
    }

    /** Put a string of characters starting at the given coordinate.
    Note that behavior is undefined if the string contains characters in 00-1F range
    or if the string includes multibyte characters.
    */
    void putline(const std::string& s, std::size_t x, std::size_t y);

    /* Put a 2D string starting at the given coordinate */
    void putbox(std::size_t x, std::size_t y, const TextBox& b);
    TextBox PutBoxCopy(std::size_t x, std::size_t y, const TextBox& b) const;

    /* Delete trailing blank from the bottom and right edges */
    void trim();

    /* Calculate the current dimensions of the string */
    std::size_t height() const;
    std::size_t width()  const;

    // width x height
    std::pair<std::size_t, std::size_t> Size() const;

    /** Draw a horizontal line.
    If bef=true, the line starts from the left edge of the first character cell, otherwise it starts from its center.
    If aft=true, the line ends in the right edge of the last character cell, otherwise it ends in its center */
    void hline(std::size_t x, std::size_t y, std::size_t width, bool bef, bool aft);

    /** Draw a vertical line.
    If bef=true, the line starts from the top edge of the first character cell, otherwise it starts from its center.
    If aft=true, the line ends in the bottom edge of the last character cell, otherwise it ends in its center */
    void vline(std::size_t x, std::size_t y, std::size_t height, bool bef, bool aft);

    /** Calculate the earliest X coordinate where the given box could be placed.
     without colliding with existing content in this box. Guaranteed to be <= width().
     Find leftmost position where box b can be appended into *this without overlap
     */
    std::size_t horiz_append_position(std::size_t y, const TextBox& b) const;

    /** Calculate the earliest Y coordinate where the given box could be placed without colliding with existing content in this box. Guaranteed to be <= height().
     * Find topmost position where box b can be appended into *this without overlap
     */
    std::size_t vert_append_position(std::size_t x, const TextBox& b) const;

    /** Converts the contents of the box into a std::string with linefeeds and VT100 escapes.
    If enable_vt100 is false, renders using plain ASCII instead.
    */
    std::vector<std::string> to_string(const TextBoxStyle& style = TerminalStyle()) const;
private:
    std::size_t FindLeftPadding(std::size_t y) const;
    std::size_t FindRightPadding(std::size_t y) const;
    std::size_t FindTopPadding(std::size_t x) const;
    std::size_t FindBottomPadding(std::size_t x) const;
};

namespace detail
{
    void CreateTreeGraph
    (
        TextBox& result,
        size_t maxwidth,
        const std::vector<TextBox>& boxes,
        bool oneliner_test,
        bool simple_test,
        bool separate1st_test,
        std::string atom,
        std::size_t margin,
        std::size_t firstx
    );
}

/* An utility function that can be used to create a tree graph rendering from a structure.
 *
 * Parameters:
 *   e:                The element that will be rendered.
 *                     Possibly some user-defined type that represents a node in a tree structure.
 *   maxwidth:         The maximum width of the resulting box in characters.
 *   create_atom:      A functor of type std::string(const ParamType&).
 *                     It renders the given element into an 1D string.
 *                     Note that the string must not contain multibyte characters,
 *                     because size() will be used to determine its width in columns.
 *   count_children:   A functor of type std::pair<ForwardIterator,ForwardIterator>(const ParamType&).
 *                     It returns a pair of iterators representing the range of children
 *                     for the given element.
 *                     create_tree_graph will call itself recursively for each element in this range.
 *   oneliner_test:    A functor of type bool(const ParamType&).
 *                     If the result is true, enables simplified horizontal topology.
 *   simple_test:      A functor of type bool(const ParamType&).
 *                     If the result is true, enables very simplified horizontal topology.
 *   separate1st_test: A functor of type bool(const ParamType&).
 *                     If the result is true, create_tree_graph() will always render
 *                     the first child alone on a separate line, but the rest of them
 *                     may get rendered horizontally.
 * 
 * @param margin  the spacing between children
 * @param firstx  the first child offset
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

template<typename ParamType, typename AtomCreator, typename ParamCountFunc,
         typename OneLinerFunc, typename SimpleTestFunc, typename Separate1stParamTestFunc>
TextBox create_tree_graph(const ParamType& e,
                          std::size_t maxwidth,
                          AtomCreator&&    create_atom,
                          ParamCountFunc&& count_children,
                          OneLinerFunc&&   oneliner_test,
                          SimpleTestFunc&& simple_test,
                          Separate1stParamTestFunc&& separate1st_test,
                          std::size_t margin = 4,
                          std::size_t firstx = 2
                          )
{
    TextBox result;
    const std::string atom = create_atom(e);
    
    result.putline(atom, 0,0);

    if(auto param_range = count_children(e); param_range.first != param_range.second)
    {
        std::vector<TextBox> boxes;

        boxes.reserve(std::distance(param_range.first, param_range.second));
        for(auto i = param_range.first; i != param_range.second; ++i)
        {
            boxes.emplace_back(create_tree_graph(*i, (maxwidth >= (16+2)) ? maxwidth - 2 : 16,
                                                 create_atom, count_children, oneliner_test, simple_test,
                                                 separate1st_test, margin, firstx));
        }
        detail::CreateTreeGraph(result, maxwidth, boxes, oneliner_test(e), simple_test(e), separate1st_test(e), atom, margin, firstx);
    }
    result.trim();
    return result;
}

}

#endif  // EUPHORIA_CORE_TEXTBOX_H