#include "core/textbox.h"

#include <cstdlib>
#include <numeric>

#include "core/cint.h"
#include "assert/assert.h"
#include "core/stringutils.h"

namespace
{
    bool
    is_terminal_supporting_utf8()
    {
        const char* clang = std::getenv("LANG");

        if(clang != nullptr)
        {
            const auto lang = std::string(clang);
            const auto lower = euphoria::core::to_lower(lang);
            const auto ends = euphoria::core::ends_with(lower, "utf-8");
            return ends;
        }

        return false;
    }


    bool
    has_char(char c)
    {
        return (c & euphoria::core::bit_no_line) != 0;
    }


    bool
    is_emtpy(char c)
    {
        return c ==' ' || c == 0;
    }
}


namespace euphoria::core
{
    std::string
    TextBoxStyle::get_string(char c) const
    {
        ASSERTX(c>0, c);
        ASSERTX
        (
            c-1 < c_sizet_to_int(connections.size()),
            from_char_to_string(c, CharToStringStyle::include_hex),
            connections.size()
        );
        return connections[c-1];
    }


    TextBoxStyle
    TextBoxStyle::create(std::function<std::string(char)> connections_func)
    {
        euphoria::core::TextBoxStyle style;
        for(char c=1; c<16; c+=1)
        {
            ASSERTX
            (
                c-1 < c_sizet_to_int(style.connections.size()),
                from_char_to_string(c, CharToStringStyle::include_hex),
                style.connections.size()
            );
            style.connections[c-1] = connections_func(c);
        }
        return style;
    }


    TextBoxStyle
    get_terminal_style()
    {
        if(is_terminal_supporting_utf8())
        {
            // return Utf8StraightStyle();
            return utf8_rounded_style();
        }
        else
        {
            return ascii_style();
        }
    }


    TextBoxStyle::TextBoxStyle()
    {
        connections.resize(15);
    }

    // todo(Gustav): make constexpr

    TextBoxStyle
    utf8_straight_style()
    {
        return TextBoxStyle::create([](char c)
        {
            switch(c)
            {
            case bit_left:
            case bit_right:
            case bit_left | bit_right:                     return u8"─";
            case bit_up:
            case bit_down:
            case bit_up | bit_down:                        return u8"│";
            case bit_left| bit_up:                         return u8"┘";
            case bit_left | bit_down:                      return u8"┐";
            case bit_right | bit_up:                       return u8"└";
            case bit_right | bit_down:                     return u8"┌";
            case bit_left | bit_right | bit_up:            return u8"┴";
            case bit_left | bit_right | bit_down:          return u8"┬";
            case bit_left | bit_up | bit_down:             return u8"┤";
            case bit_right | bit_up | bit_down:            return u8"├";
            case bit_left | bit_right | bit_up | bit_down: return u8"┼";
            default:
                DIE("Invalid combination");
                return "X";
            }
        });
    }

    TextBoxStyle
    utf8_rounded_style()
    {
        return TextBoxStyle::create([](char c)
        {
            switch(c)
            {
            case bit_left:
            case bit_right:
            case bit_left | bit_right:                     return u8"─";
            case bit_up:
            case bit_down:
            case bit_up | bit_down:                        return u8"│";
            case bit_left| bit_up:                         return u8"╯";
            case bit_left | bit_down:                      return u8"╮";
            case bit_right | bit_up:                       return u8"╰";
            case bit_right | bit_down:                     return u8"╭";
            case bit_left | bit_right | bit_up:            return u8"┴";
            case bit_left | bit_right | bit_down:          return u8"┬";
            case bit_left | bit_up | bit_down:             return u8"┤";
            case bit_right | bit_up | bit_down:            return u8"├";
            case bit_left | bit_right | bit_up | bit_down: return u8"┼";
            default:
                DIE("Invalid combination");
                return "X";
            }
        });
    }

    TextBoxStyle
    utf_8double_line_style()
    {
        return TextBoxStyle::create([](char c)
        {
            switch(c)
            {
            case bit_left:
            case bit_right:
            case bit_left | bit_right:                     return u8"═";
            case bit_up:
            case bit_down:
            case bit_up | bit_down:                        return u8"║";
            case bit_left| bit_up:                         return u8"╝";
            case bit_left | bit_down:                      return u8"╗";
            case bit_right | bit_up:                       return u8"╚";
            case bit_right | bit_down:                     return u8"╔";
            case bit_left | bit_right | bit_up:            return u8"╩";
            case bit_left | bit_right | bit_down:          return u8"╦";
            case bit_left | bit_up | bit_down:             return u8"╣";
            case bit_right | bit_up | bit_down:            return u8"╠";
            case bit_left | bit_right | bit_up | bit_down: return u8"╬";
            default:
                DIE("Invalid combination");
                return "X";
            }
        });
    }

    TextBoxStyle
    ascii_style()
    {
        return TextBoxStyle::create([](char c)
        {
            switch(c)
            {
            case bit_left:
            case bit_right:
            case bit_left | bit_right:                     return "-";
            case bit_up:
            case bit_down:
            case bit_up | bit_down:                        return "|";
            case bit_left| bit_up:                         return "'";
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
        });
    }


    TextBox::TextBox() = default;


    TextBox
    TextBox::create_empty()
    {
        return {};
    }


    TextBox
    TextBox::create_from_strings(const std::vector<std::string>& str)
    {
        TextBox ret;
        ret.data = str;
        return ret;
    }


    void
    TextBox::put_char(int x, int y, char c)
    {
        extend_to(x,y);
        data[y][x] = c;
    }


    void
    TextBox::extend_to(int x, int y)
    {
        if(y >= c_sizet_to_int(data.size()))
        {
            data.resize(y+1);
        }
        if(c_sizet_to_int(data[y].size()) <= x)
        {
            data[y].resize(x+1, ' ');
        }
    }


    void
    TextBox::put_string
    (
        int x,
        int y,
        const std::string& line
    )
    {
        for(int index = 0; index < c_sizet_to_int(line.length()); index+=1)
        {
            put_char(x+index, y, line[index]);
        }
    }

    TextBox
    TextBox::from_string(const std::string& s, int x, int y)
    {
        auto tb = create_empty();
        tb.put_string(x, y, s);
        return tb;
    }


    void
    TextBox::put_box
    (
        int x_start,
        int y_start,
        const TextBox& b
    )
    {
        for(int p = 0; p < c_sizet_to_int(b.data.size()); p+=1)
        {
            const auto line = b.data[p];
            const auto y = y_start + p;

            const int size_minus_1 = line.empty() ? 0 : c_sizet_to_int(line.size())-1;
            extend_to(x_start+size_minus_1, y);

            for(int line_index = 0; line_index < c_sizet_to_int(line.size()); line_index+=1)
            {
                const auto x = x_start + line_index;
                const auto source_texel = line[line_index];

                ASSERT(x < c_sizet_to_int(data[y].size()));

                if(!is_emtpy(source_texel))
                {
                    char& target = data[y][x];
                    if(is_emtpy(target) || has_char(source_texel))
                    {
                        target = source_texel;
                    }
                    else
                    {
                        if( has_char(target) )
                        {
                            target = 0;
                        }
                        target = static_cast<char>(target | source_texel);
                    }
                }
            }
        }
    }


    TextBox
    TextBox::put_box_copy
    (
        int x,
        int y,
        const TextBox& b
    ) const
    {
        TextBox self = *this;
        self.put_box(x, y, b);
        return self;
    }


    void
    TextBox::trim()
    {
        for(auto& s: data)
        {
            int end = c_sizet_to_int(s.size());
            while(end > 0 && is_emtpy(s[end - 1]))
            {
                end-=1;
            }
            s.erase(end);
        }

        while(!data.empty() && data.back().empty())
        {
            data.pop_back();
        }
    }


    int
    TextBox::get_height() const
    {
        return c_sizet_to_int(data.size());
    }


    int
    TextBox::get_width() const
    {
        int result = 0;

        for(const auto& s: data)
        {
            result = std::max(result, c_sizet_to_int(s.size()));
        }

        return result;
    }


    std::pair<int, int>
    TextBox::get_size() const
    {
        return {get_width(), get_height()};
    }


    void
    TextBox::put_horizontal_line
    (
        int x,
        int y,
        int line_width,
        bool bef,
        bool aft
    )
    {
        for(int line_index=0; line_index<line_width; line_index+=1)
        {
            mod_char(x+line_index, y, [&](char& c)
            {
                if( has_char(c) )
                {
                    c = 0;
                }

                if(line_index>0 || bef)
                {
                    c |= bit_left;
                }

                if(aft || (line_index+1)<line_width)
                {
                    c |= bit_right;
                }
            });
        }
    }


    void
    TextBox::put_vertical_line
    (
        int x,
        int y,
        int line_height,
        bool bef,
        bool aft
    )
    {
        for(int line_index=0; line_index<line_height; line_index+=1)
        {
            mod_char(x, y+line_index, [&](char& c)
            {
                if( has_char(c) )
                {
                    c = 0;
                }

                if(line_index>0 || bef)
                {
                    c |= bit_up;
                }

                if(aft || (line_index+1)<line_height)
                {
                    c |= bit_down;
                }
            });
        }
    }


    int
    TextBox::get_horizontal_append_position(int y, const TextBox& b) const
    {
        const int my_width = get_width();

        int reduce = my_width;
        for(int p=0; p<b.get_height(); p+=1)
        {
            reduce = std::min(reduce, find_right_padding(y+p) + b.find_left_padding(p));
        }

        return my_width - reduce;
    }


    int
    TextBox::get_vertical_append_position
    (
        int x,
        const TextBox& b
    ) const
    {
        const int my_height = get_height();

        int reduce = my_height;
        for(int p=0; p<b.get_width(); p+=1)
        {
            reduce = std::min(reduce, find_bottom_padding(x+p) + b.find_top_padding(p));
        }

        return my_height - reduce;
    }


    std::vector<std::string>
    TextBox::to_string(const TextBoxStyle& style) const
    {
        std::vector<std::string> ret;
        bool want_newline = true;
        auto last_line = [&ret, &want_newline]() -> std::string&
        {
            if(want_newline)
            {
                ret.emplace_back("");
                want_newline = false;
            }

            return ret[ret.size()-1];
        };

        auto append = [&](char c)
        {
            if(c == '\n')
            {
                if(want_newline)
                {
                    // if we want another newline, add the one before first
                    last_line();
                }
                want_newline = true;
            }
            else
            {
                last_line() += c;
            }
        };

        const int h = get_height();
        for(int y = 0; y < h; y+=1)
        {
            const std::string& s = data[y];
            for(int x = 0; x < c_sizet_to_int(s.size()); x+=1)
            {
                char c = s[x];
                if(c > 0 && c < 16)
                {
                    const auto str = style.get_string(c);
                    for(auto line_char: str)
                    {
                        append(line_char);
                    }
                }
                else
                {
                append(c);
                }
            }
            append('\n');
        }
        return ret;
    }


    int
    TextBox::find_left_padding(int y) const
    {
        const int max = get_width();

        if(y >= c_sizet_to_int(data.size()))
        {
            return max;
        }

        const std::string& line = data[y];

        int result = 0;
        while(result < c_sizet_to_int(line.size()) && is_emtpy(line[result]))
        {
            result+=1;
        }

        return result;
    }


    int
    TextBox::find_right_padding(int y) const
    {
        const int max = get_width();

        if(y >= c_sizet_to_int(data.size()))
        {
            return max;
        }

        const std::string& line = data[y];
        int position = max;
        int result = 0;
        while
        (
            position != 0 &&
            (position-1 >= c_sizet_to_int(line.size()) || is_emtpy(line[position - 1]))
        )
        {
            position-=1;
            result+=1;
        }

        return result;
    }


    int
    TextBox::find_bottom_padding(int x) const
    {
        const int max = c_sizet_to_int(data.size());

        int result = 0;
        int position = max;

        while
        (
            position != 0 &&
            (x >= c_sizet_to_int(data[position-1].size()) || is_emtpy(data[position - 1][x]))
        )
        {
            position-=1;
            result+=1;
        }

        return result;
    }


    int
    TextBox::find_top_padding(int x) const
    {
        const int max = c_sizet_to_int(data.size());
        int result = 0;

        while
        (
            result < max &&
            (x >= c_sizet_to_int(data[result].size()) || is_emtpy(data[result][x]))
        )
        {
            result+=1;
        }

        return result;
    }


    void
    TextBox::create_tree_graph_impl
    (
        TextBox* result,
        int maxwidth,
        const std::vector<TextBox>& boxes,
        bool consider_oneliner,
        bool consider_simple,
        const std::string& label,
        int margin,
        int firstx
    )
    {
        constexpr int min_y = 1;

        const int totalwidth = boxes.empty() ? 0 :
            std::accumulate
            (
                boxes.begin(),
                boxes.end(),
                0,
                [](auto t, const auto& b) {return t + b.get_width();}
            ) + (c_sizet_to_int(boxes.size())-1) * margin
        ;

        bool oneliner = consider_oneliner &&
            (c_sizet_to_int(label.size()) + margin + totalwidth) < maxwidth;
        bool simple = consider_simple && oneliner && boxes.size() == 1;

        int y = simple ? 0 : 1;

        for(auto box_iterator = boxes.begin(); box_iterator != boxes.end(); box_iterator+=1)
        {
            const TextBox& current_box = *box_iterator;
            const int usemargin = (simple || oneliner) ? (margin/2) : margin;
            const auto first_valid_x = result->get_horizontal_append_position(y, current_box);
            int x = first_valid_x != 0 ? first_valid_x + usemargin
                :(
                    oneliner
                    ? c_sizet_to_int(label.size())+usemargin
                    : firstx
                );

            if(!oneliner && (x + current_box.get_width() > maxwidth))
            {
                // Start a new line if this item won't fit in the
                // end of the current line
                x = firstx;
                simple = false;
                oneliner = false;
            }

            // At the beginning of line, judge whether to add room for
            // horizontal placement
            bool horizontal = x > firstx;
            if
            (
                !oneliner &&
                !horizontal &&
                std::next(box_iterator) != boxes.end()
            )
            {
                const auto& next_box = *std::next(box_iterator);
                int combined_width =
                    current_box.get_horizontal_append_position(0, next_box) +
                    margin +
                    next_box.get_width()
                ;
                if(combined_width <= maxwidth)
                {
                    // Enact horizontal placement by giving 1 row of room
                    // for the connector
                    horizontal = true;
                    const TextBox combined = current_box.put_box_copy
                    (
                        current_box.get_horizontal_append_position(0, next_box) + margin,
                        0,
                        next_box
                    );
                    y = std::max
                    (
                        result->get_vertical_append_position(x, combined),
                        1
                    );
                    if(!oneliner)
                    {
                        y+=1;
                    }
                }
            }
            if(!horizontal)
            {
                y = std::max
                (
                    result->get_vertical_append_position(x, current_box),
                    1
                );
            }
            if(horizontal && !simple && !oneliner)
            {
                while(true)
                {
                    // Check if there is room for a horizontal connector.
                    // If not, increase y
                    const auto connector = TextBox::from_string
                    (
                        std::string(1+x, '-')
                    );
                    if(result->get_horizontal_append_position(y-1, connector) > x)
                    {
                        y+=1;
                    }
                    else
                    {
                        break;
                    }
                    y = std::max
                    (
                        result->get_vertical_append_position(x, current_box),
                        y
                    );
                }
            }

            if(simple)
            {
            }
            else
            {
                y = std::max(y, min_y);
            }

            if(simple)
            {
                if(x > c_sizet_to_int(label.size()))
                {
                    const auto label_size = c_sizet_to_int(label.size());
                    result->put_horizontal_line
                    (
                        label_size,
                        0,
                        1+x-label_size,
                        false,
                        false
                    );
                }
            }
            else if(oneliner)
            {
                const auto label_size = c_sizet_to_int(label.size());

                int cx = x;
                int cy = y > min_y ? y-min_y : 0;
                if(x > c_sizet_to_int(label.size()))
                {
                    result->put_horizontal_line
                    (
                        label_size,
                        0,
                        1+x-label_size,
                        false,
                        false
                    );
                }
                result->put_vertical_line(cx, cy, min_y, false,true);
            }
            else if(horizontal)
            {
                int cx = x;
                int cy = y-1;
                result->put_vertical_line(0,  1,  1 + (cy-1), true,false);
                result->put_horizontal_line(0, cy, 1 + (cx-0), false,false);
                result->put_vertical_line(cx, cy, 1, false,true);
            }
            else
            {
                int cx = x-1;
                int cy = y;
                result->put_vertical_line(0,1, 1 + (cy-1), true,false);
                result->put_horizontal_line(0,cy, 1 + (cx-0), false,true);
            }

            result->put_box(x, y, current_box);
        }
    }
}

