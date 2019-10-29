#include <string>
#include <vector>
#include <algorithm>

namespace euphoria::core
{

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
     */
    std::size_t horiz_append_position(std::size_t y, const TextBox& b) const;

    /** Calculate the earliest Y coordinate where the given box could be placed without colliding with existing content in this box. Guaranteed to be <= height().
     */
    std::size_t vert_append_position(std::size_t x, const TextBox& b) const;

    /** Converts the contents of the box into a std::string with linefeeds and VT100 escapes.
    If enable_vt100 is false, renders using plain ASCII instead.
    */
    std::vector<std::string> to_string(bool enable_vt100 = false) const;
private:
    std::size_t FindLeftPadding(std::size_t y) const;
    std::size_t FindRightPadding(std::size_t y) const;
    std::size_t FindTopPadding(std::size_t x) const;
    std::size_t FindBottomPadding(std::size_t x) const;
};

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
                          Separate1stParamTestFunc&& separate1st_test)
{
    TextBox result;
    std::string atom = create_atom(e);
    result.putline(atom, 0,0);

    if(auto param_range = count_children(e); param_range.first != param_range.second)
    {
        std::vector<TextBox> boxes;
        boxes.reserve(std::distance(param_range.first, param_range.second));
        for(auto i = param_range.first; i != param_range.second; ++i)
            boxes.emplace_back(create_tree_graph(*i, (maxwidth >= (16+2)) ? maxwidth - 2 : 16,
                                                 create_atom, count_children, oneliner_test, simple_test,
                                                 separate1st_test));

        constexpr std::size_t margin = 4, firstx = 2;

        std::size_t sum_width = 0;
        for(const auto& b: boxes) sum_width += b.width()+margin;

        bool oneliner = false;
        if(oneliner_test(e) && !separate1st_test(e))
        {
            std::size_t totalwidth = 0;
            for(auto i = boxes.begin(); ; )
            {
                const auto& cur = *i;
                if(++i == boxes.end()) { totalwidth += cur.width(); break; }
                //const auto& next = *i;
                totalwidth += cur.width()/*cur.horiz_append_position(0, next)*/ + margin;
            }
            oneliner = (atom.size() + margin + totalwidth) < maxwidth;
        }
        bool simple = oneliner && boxes.size() == 1 && simple_test(e); // ret, addrof, etc.

        std::size_t y = simple ? 0 : 1;

        for(auto i = boxes.begin(); i != boxes.end(); ++i)
        {
            auto next = ++std::vector<TextBox>::iterator(i);
            const TextBox& cur = *i;
            unsigned width = cur.width();

            std::size_t usemargin = (simple || oneliner) ? (margin/2) : margin;
            std::size_t x = result.horiz_append_position(y, cur) + usemargin;
            if(x==usemargin) x = oneliner ? atom.size()+usemargin : firstx;
            if(!oneliner && (x + width > maxwidth || (separate1st_test(e) && i == ++boxes.begin())))
            {
                // Start a new line if this item won't fit in the end of the current line
                x        = firstx;
                simple   = false;
                oneliner = false;
            }

            // At the beginning of line, judge whether to add room for horizontal placement
            bool horizontal = x > firstx;
            if(!oneliner && !horizontal && next != boxes.end() && !(separate1st_test(e) && i == boxes.begin()))
            {
                std::size_t nextwidth      = next->width();
                std::size_t combined_width = cur.horiz_append_position(0, *next) + margin + nextwidth;
                if(combined_width <= maxwidth)
                {
                    // Enact horizontal placement by giving 1 row of room for the connector
                    horizontal = true;
                    TextBox combined = cur;
                    combined.putbox(cur.horiz_append_position(0, *next) + margin, 0, *next);
                    y = std::max(result.vert_append_position(x, combined), std::size_t(1));
                    if(!oneliner) ++y;
                }
            }
            if(!horizontal)
                y = std::max(result.vert_append_position(x, cur), std::size_t(1));
            if(horizontal && !simple && !oneliner)
                for(;;)
                {
                    // Check if there is room for a horizontal connector. If not, increase y
                    TextBox conn;
                    conn.putline(std::string(1+(x-0), '-'), 0, 0);
                    if(result.horiz_append_position(y-1, conn) > x) ++y; else break;
                    y = std::max(result.vert_append_position(x, cur), y);
                }

            if(simple)
            {
                if(x > atom.size())
                    result.hline(atom.size(), 0, 1+x-atom.size(), false,false);
            }
            else if(oneliner)
            {
                unsigned cx = x, cy = y-1;
                if(x > atom.size())
                    result.hline(atom.size(), 0, 1+x-atom.size(), false,false);
                result.vline(cx, cy, 1,          false,true);
            }
            else if(horizontal)
            {
                unsigned cx = x, cy = y-1;
                result.vline(0,  1,  1 + (cy-1), true,false);
                result.hline(0,  cy, 1 + (cx-0), false,false);
                result.vline(cx, cy, 1,          false,true);
            }
            else
            {
                unsigned cx = x-1, cy = y;
                result.vline(0,1,  1 + (cy-1), true,false);
                result.hline(0,cy, 1 + (cx-0), false,true);
            }

            result.putbox(x, y, cur);
        }
    }
    result.trim();
    return result;
}

}
