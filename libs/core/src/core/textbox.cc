#include "core/textbox.h"

#include <cstdlib>
#include <numeric>

#include "core/cint.h"
#include "core/assert.h"
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
            char_to_string(c, CharToStringStyle::include_hex),
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
                char_to_string(c, CharToStringStyle::include_hex),
                style.connections.size()
            );
            style.connections[c-1] = connections_func(c);
        }
        return style;
    }


    TextBoxStyle
    terminal_style()
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
    TextBox::put_char(std::size_t x, std::size_t y, char c)
    {
        extend_to(x,y);
        data[y][x] = c;
    }


    void
    TextBox::extend_to(std::size_t x, std::size_t y)
    {
        if(y >= data.size())
        {
            data.resize(y+1);
        }
        if(data[y].size() <= x)
        {
            data[y].resize(x+1, ' ');
        }
    }


    void
    TextBox::put_string
    (
        std::size_t x,
        std::size_t y,
        const std::string& line
    )
    {
        for(std::size_t index = 0; index < line.length(); index+=1)
        {
            put_char(x+index, y, line[index]);
        }
    }

    TextBox
    TextBox::from_string(const std::string& s, std::size_t x, std::size_t y)
    {
        auto tb = create_empty();
        tb.put_string(x, y, s);
        return tb;
    }


    void
    TextBox::put_box
    (
        std::size_t x_start,
        std::size_t y_start,
        const TextBox& b
    )
    {
        for(std::size_t p = 0; p < b.data.size(); p+=1)
        {
            const auto line = b.data[p];
            const auto y = y_start + p;

            const auto size_minus_1 = line.empty() ? 0 : line.size()-1;
            extend_to(x_start+size_minus_1, y);

            for(std::size_t line_index = 0; line_index < line.size(); line_index+=1)
            {
                const auto x = x_start + line_index;
                const auto source_texel = line[line_index];

                ASSERT(x < data[y].size());

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
        std::size_t x,
        std::size_t y,
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
            std::size_t end = s.size();
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


    std::size_t
    TextBox::get_height() const
    {
        return data.size();
    }


    std::size_t
    TextBox::get_width() const
    {
        std::size_t result = 0;

        for(const auto& s: data)
        {
            result = std::max(result, s.size());
        }

        return result;
    }


    std::pair<std::size_t, std::size_t>
    TextBox::get_size() const
    {
        return {get_width(), get_height()};
    }


    void
    TextBox::put_horizontal_line
    (
        std::size_t x,
        std::size_t y,
        std::size_t line_width,
        bool bef,
        bool aft
    )
    {
        for(std::size_t line_index=0; line_index<line_width; line_index+=1)
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
        std::size_t x,
        std::size_t y,
        std::size_t line_height,
        bool bef,
        bool aft
    )
    {
        for(std::size_t line_index=0; line_index<line_height; line_index+=1)
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


    std::size_t
    TextBox::get_horizontal_append_position(std::size_t y, const TextBox& b) const
    {
        const std::size_t my_width = get_width();

        std::size_t reduce = my_width;
        for(std::size_t p=0; p<b.get_height(); p+=1)
        {
            reduce = std::min(reduce, find_right_padding(y+p) + b.find_left_padding(p));
        }

        return my_width - reduce;
    }


    std::size_t
    TextBox::get_vertical_append_position
    (
        std::size_t x,
        const TextBox& b
    ) const
    {
        const std::size_t my_height = get_height();

        std::size_t reduce = my_height;
        for(std::size_t p=0; p<b.get_width(); p+=1)
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

        const std::size_t h = get_height();
        for(std::size_t y = 0; y < h; y+=1)
        {
            const std::string& s = data[y];
            for(std::size_t x = 0; x < s.size(); x+=1)
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


    std::size_t
    TextBox::find_left_padding(std::size_t y) const
    {
        const std::size_t max = get_width();

        if(y >= data.size())
        {
            return max;
        }

        const std::string& line = data[y];

        std::size_t result = 0;
        while(result < line.size() && is_emtpy(line[result]))
        {
            result+=1;
        }

        return result;
    }


    std::size_t
    TextBox::find_right_padding(std::size_t y) const
    {
        const std::size_t max = get_width();

        if(y >= data.size())
        {
            return max;
        }

        const std::string& line = data[y];
        std::size_t position = max;
        std::size_t result = 0;
        while
        (
            position != 0 &&
            (position-1 >= line.size() || is_emtpy(line[position - 1]))
        )
        {
            position-=1;
            result+=1;
        }

        return result;
    }


    std::size_t
    TextBox::find_bottom_padding(std::size_t x) const
    {
        const std::size_t max = data.size();

        std::size_t result = 0;
        std::size_t position = max;

        while
        (
            position != 0 &&
            (x >= data[position-1].size() || is_emtpy(data[position - 1][x]))
        )
        {
            position-=1;
            result+=1;
        }

        return result;
    }



    std::size_t
    TextBox::find_top_padding(std::size_t x) const
    {
        const std::size_t max = data.size();
        std::size_t result = 0;

        while
        (
            result < max &&
            (x >= data[result].size() || is_emtpy(data[result][x]))
        )
        {
            result+=1;
        }

        return result;
    }


    void
    TextBox::sub_create_tree_graph
    (
        TextBox* result,
        size_t maxwidth,
        const std::vector<TextBox>& boxes,
        bool consider_oneliner,
        bool consider_simple,
        const std::string& label,
        std::size_t margin,
        std::size_t firstx
    )
    {
        constexpr std::size_t min_y = 1;

        const auto totalwidth = boxes.empty() ? 0 :
            std::accumulate
            (
                boxes.begin(),
                boxes.end(),
                0,
                [](auto t, const auto& b) {return t + b.get_width();}
            ) + (boxes.size()-1) * margin
        ;

        bool oneliner = consider_oneliner &&
            (label.size() + margin + totalwidth) < maxwidth;
        bool simple = consider_simple && oneliner && boxes.size() == 1;

        std::size_t y = simple ? 0 : 1;

        for(auto box_iterator = boxes.begin(); box_iterator != boxes.end(); box_iterator+=1)
        {
            const TextBox& current_box = *box_iterator;
            const std::size_t usemargin = (simple || oneliner) ? (margin/2) : margin;
            const auto first_valid_x = result->get_horizontal_append_position(y, current_box);
            std::size_t x = first_valid_x != 0 ? first_valid_x + usemargin
                :(
                    oneliner
                    ? label.size()+usemargin
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
                std::size_t combined_width =
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
                        std::size_t(1)
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
                    std::size_t(1)
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
                if(x > label.size())
                {
                    result->put_horizontal_line
                    (
                        label.size(),
                        0,
                        1+x-label.size(),
                        false,
                        false
                    );
                }
            }
            else if(oneliner)
            {
                unsigned cx = x;
                unsigned cy = y > min_y ? y-min_y : 0;
                if(x > label.size())
                {
                    result->put_horizontal_line
                    (
                        label.size(),
                        0,
                        1+x-label.size(),
                        false,
                        false
                    );
                }
                result->put_vertical_line(cx, cy, min_y, false,true);
            }
            else if(horizontal)
            {
                unsigned cx = x;
                unsigned cy = y-1;
                result->put_vertical_line(0,  1,  1 + (cy-1), true,false);
                result->put_horizontal_line(0, cy, 1 + (cx-0), false,false);
                result->put_vertical_line(cx, cy, 1, false,true);
            }
            else
            {
                unsigned cx = x-1;
                unsigned cy = y;
                result->put_vertical_line(0,1, 1 + (cy-1), true,false);
                result->put_horizontal_line(0,cy, 1 + (cx-0), false,true);
            }

            result->put_box(x, y, current_box);
        }
    }
}

