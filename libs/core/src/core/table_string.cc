#include "core/table_string.h"

#include <algorithm>

#include "core/stringutils.h"


namespace euphoria::core
{
    string_table
    table_from_csv(const std::string& csv_data, const csv_parser_options& options)
    {
        const auto add_row_to_table
                = [](string_table* table, const std::vector<std::string>& row) {
                      if(row.empty())
                      {
                          return;
                      }
                      table->new_row(row);
                  };
        auto file = TextFileParser::FromString(csv_data);

        string_table table;

        std::vector<std::string> row;
        std::stringstream        ss;
        bool                     inside_string   = false;
        bool                     added           = false;
        bool                     was_from_string = false;

        while(file.HasMore())
        {
            const auto c = file.ReadChar();
            if(inside_string)
            {
                if(c == options.str)
                {
                    if(file.PeekChar() == options.str)
                    {
                        ss << c;
                        file.ReadChar();
                    }
                    else
                    {
                        inside_string = false;
                    }
                }
                else
                {
                    ss << c;
                }
            }
            else
            {
                if(c == options.str)
                {
                    if(added)
                    {
                        // todo(Gustav): generate error if string contains stuff other than whitespace
                        ss.str("");
                    }
                    inside_string   = true;
                    was_from_string = true;
                    added           = true;
                }
                else if(c == options.delim)
                {
                    auto column_data = ss.str();
                    if(!was_from_string && options.trim == csv_trim::trim)
                    {
                        column_data = trim(column_data);
                    }
                    row.push_back(column_data);
                    ss.str("");
                    added           = false;
                    was_from_string = false;
                }
                else if(c == '\n')
                {
                    if(added)
                    {
                        auto column_data = ss.str();
                        if(!was_from_string && options.trim == csv_trim::trim)
                        {
                            column_data = trim(column_data);
                        }
                        row.push_back(column_data);
                    }
                    ss.str("");
                    add_row_to_table(&table, row);
                    row.resize(0);
                    added           = false;
                    was_from_string = false;
                }
                else
                {
                    if(was_from_string)
                    {
                        // skip
                        // todo(Gustav): error on non whitespace?
                    }
                    else
                    {
                        ss << c;
                        added = true;
                    }
                }
            }
        }

        if(added)
        {
            auto column_data = ss.str();
            if(!was_from_string && options.trim == csv_trim::trim)
            {
                column_data = trim(column_data);
            }
            row.push_back(column_data);
        }
        add_row_to_table(&table, row);

        return table;
    }

    int
    width_of_string(const std::string& t)
    {
        // todo(Gustav): bugfix longstring\nshort will return length of short, not longstring
        int w = 0;

        for(auto c: t)
        {
            if(c == '\n')
            {
                w = 0;
            }
            else
            {
                w += 1;
            }
        }

        return w;
    }

    int
    height_of_string(const std::string& t)
    {
        int h = 1;
        for(auto c: t)
        {
            if(c == '\n')
            {
                h += 1;
            }
        }
        return h;
    }

    int
    column_width(const string_table& t, int c)
    {
        int width = 0;
        for(string_table::I y = 0; y < t.get_height(); y += 1)
        {
            width = std::max<int>(width, width_of_string(t(c, y)));
        }
        return width;
    }

    int
    row_height(const string_table& t, int r)
    {
        int height = 0;
        for(string_table::I x = 0; x < t.get_width(); x += 1)
        {
            height = std::max<int>(height, height_of_string(t(x, r)));
        }
        return height;
    }

    std::vector<int>
    column_widths(const string_table& table, int extra)
    {
        const auto       number_of_cols = table.get_width();
        std::vector<int> sizes(number_of_cols);
        for(string_table::I i = 0; i < number_of_cols; ++i)
        {
            sizes[i] = column_width(table, i) + extra;
        }
        return sizes;
    }

    /// Return a new table based on row, where each cell is split at newline over many rows
    string_table
    split_table_cells_on_newline(const string_table& table, string_table::I row)
    {
        auto ret = string_table::from_width_height(
                table.get_width(), row_height(table, row));
        for(int c = 0; c < table.get_width(); c += 1)
        {
            const auto rows = split(table(c, row), '\n');
            for(string_table::I i = 0; i < string_table::Ci(rows.size()); i += 1)
            {
                ret(c, i) = rows[i];
            }
        }
        return ret;
    }

    void
    print_table_simple(std::ostream& out, const string_table& maintable)
    {
        // todo(Gustav): cleanup this function...
        const auto begin_str_padding = 1;
        const auto end_space_padding = 3;

        const auto begin_str      = std::string(begin_str_padding, ' ');
        const auto end_str        = std::string(end_space_padding, ' ');
        const auto number_of_cols = maintable.get_width();
        const auto number_of_rows = maintable.get_height();

        const std::vector<int> sizes = column_widths(maintable, 1);

        const auto total_padding = begin_str_padding + end_space_padding;

        for(string_table::I mainrow = 0; mainrow < number_of_rows; ++mainrow)
        {
            const auto subtable = split_table_cells_on_newline(maintable, mainrow);
            for(string_table::I subrow = 0; subrow < subtable.get_height(); ++subrow)
            {
                for(string_table::I col = 0; col < number_of_cols; ++col)
                {
                    const auto cell = begin_str + subtable(col, subrow);
                    int        line_length = cell.length();
                    out << cell;

                    if(col != number_of_cols - 1)
                    {
                        for(string_table::I i = line_length;
                            i < sizes[col] + total_padding;
                            ++i)
                        {
                            out << ' ';
                        }
                    }
                }
                out << '\n';

                if(mainrow == 0)
                {
                    for(string_table::I col = 0; col < number_of_cols; ++col)
                    {
                        const auto row_text
                                = std::string(
                                          sizes[col] + begin_str_padding, '-')
                                  + std::string(end_space_padding, ' ');
                        out << row_text;
                    }
                    out << '\n';
                }
            }
        }
    }

    void
    print_table_grid(std::ostream& out, const string_table& maintable)
    {
        const std::vector<int> sizes = column_widths(maintable, 0);

        // prefer enum since vs2017 requires one to capture constexpr (weird I know)
        enum
        {
            internal_space = 1
        };

        auto some_space = [&out](int spaces, char c = ' ') {
            for(int i = 0; i < spaces; i += 1)
            {
                out << c;
            }
        };

        auto horizontal_line = [&some_space, &out, &sizes]() {
            out << '+';
            for(auto s: sizes)
            {
                some_space(s + internal_space * 2, '-');
                out << '+';
            }
            out << '\n';
        };

        horizontal_line();

        for(string_table::I y = 0; y < maintable.get_height(); ++y)
        {
            const auto subtable = split_table_cells_on_newline(maintable, y);
            for(string_table::I suby = 0; suby < subtable.get_height(); suby += 1)
            {
                out << "|";
                for(string_table::I x = 0; x < subtable.get_width(); ++x)
                {
                    const auto cell = subtable(x, suby);
                    some_space(internal_space);
                    out << cell;
                    some_space(sizes[x] - Csizet_to_int(cell.length()));
                    some_space(internal_space);
                    out << '|';
                }
                out << '\n';
            }

            if(y == 0)
            {
                horizontal_line();
            }
        }

        horizontal_line();
    }

}
