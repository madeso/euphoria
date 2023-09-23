#include "core/table_string.h"

#include <algorithm>

#include "base/stringutils.h"
#include "base/cint.h"
#include "base/stringbuilder.h"


namespace eu::core
{


StringTable
parse_csv(const std::string& csv_data, const CsvParserOptions& options)
{
    const auto add_row_to_table =
        [](StringTable* table, const std::vector<std::string>& row)
        {
            if(row.empty()) { return; }
            table->add_row(row);
        };
    auto file = TextfileParser::from_string(csv_data);

    StringTable table;

    std::vector<std::string> row;
    auto ss = StringBuilder{};
    bool inside_string = false;
    bool added = false;
    bool was_from_string = false;

    while(file.has_more())
    {
        const auto c = file.read_char();
        if(inside_string)
        {
            if(c == options.str)
            {
                if(file.peek_char() == options.str)
                {
                    ss.add_char(c);
                    file.read_char();
                }
                else
                {
                    inside_string = false;
                }
            }
            else
            {
                ss.add_char(c);
            }
        }
        else
        {
            if(c == options.str)
            {
                if(added)
                {
                    // todo(Gustav): generate error if string contains stuff other than whitespace
                    ss.clear();
                }
                inside_string = true;
                was_from_string = true;
                added = true;
            }
            else if(c == options.delim)
            {
                auto column_data = ss.to_string();
                if(!was_from_string && options.trim == CsvTrim::trim)
                {
                    column_data = trim(column_data);
                }
                row.push_back(column_data);
                ss.clear();
                added = false;
                was_from_string = false;
            }
            else if(c == '\n')
            {
                if(added)
                {
                    auto column_data = ss.to_string();
                    if(!was_from_string && options.trim == CsvTrim::trim)
                    {
                        column_data = trim(column_data);
                    }
                    row.push_back(column_data);
                }
                ss.clear();
                add_row_to_table(&table, row);
                row.resize(0);
                added = false;
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
                    ss.add_char(c);
                    added = true;
                }
            }
        }
    }

    if(added)
    {
        auto column_data = ss.to_string();
        if(!was_from_string && options.trim == CsvTrim::trim)
        {
            column_data = trim(column_data);
        }
        row.push_back(column_data);
    }
    add_row_to_table(&table, row);

    return table;
}

int
calc_width_of_string(const std::string& t)
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
calc_height_of_string(const std::string& t)
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
calc_single_column_width(const StringTable& t, int c)
{
    int width = 0;
    for(StringTable::Idx y = 0; y < t.get_height(); y += 1)
    {
        width = std::max<int>(width, calc_width_of_string(t[{c, y}]));
    }
    return width;
}

int
calc_single_row_height(const StringTable& t, int r)
{
    int height = 0;
    for(StringTable::Idx x = 0; x < t.get_width(); x += 1)
    {
        height = std::max<int>(height, calc_height_of_string(t[{x, r}]));
    }
    return height;
}

std::vector<int>
calc_all_column_widths(const StringTable& table, int extra)
{
    const auto number_of_cols = table.get_width();
    std::vector<int> sizes(number_of_cols);
    for(StringTable::Idx column_index = 0; column_index < number_of_cols; column_index += 1)
    {
        sizes[column_index] = calc_single_column_width(table, column_index) + extra;
    }
    return sizes;
}

/// Return a new table based on row, where each cell is split at newline over many rows
StringTable
split_table_cells_on_newline(const StringTable& table, StringTable::Idx row)
{
    auto ret = StringTable::from_width_height(
            table.get_width(), calc_single_row_height(table, row));
    for(int column_index = 0; column_index < table.get_width(); column_index += 1)
    {
        const auto rows = split(table[{column_index, row}], '\n');
        for(StringTable::Idx row_index = 0; row_index < StringTable::conv(rows.size()); row_index += 1)
        {
            ret[{column_index, row_index}] = rows[row_index];
        }
    }
    return ret;
}

void
print_table_simple(std::ostream& out, const StringTable& maintable)
{
    // todo(Gustav): cleanup this function...
    const auto begin_str_padding = 1;
    const auto end_space_padding = 3;

    const auto begin_str = std::string(begin_str_padding, ' ');
    const auto end_str = std::string(end_space_padding, ' ');
    const auto number_of_cols = maintable.get_width();
    const auto number_of_rows = maintable.get_height();

    const std::vector<int> sizes = calc_all_column_widths(maintable, 1);

    const auto total_padding = begin_str_padding + end_space_padding;

    for(StringTable::Idx mainrow = 0; mainrow < number_of_rows; ++mainrow)
    {
        const auto subtable = split_table_cells_on_newline(maintable, mainrow);
        for(StringTable::Idx subrow = 0; subrow < subtable.get_height(); ++subrow)
        {
            for(StringTable::Idx col = 0; col < number_of_cols; ++col)
            {
                const auto cell = begin_str + subtable[{col, subrow}];
                auto line_length = c_sizet_to_int(cell.length());
                out << cell;

                if(col != number_of_cols - 1)
                {
                    for(StringTable::Idx padding_counter = line_length;
                        padding_counter < sizes[col] + total_padding;
                        ++padding_counter)
                    {
                        out << ' ';
                    }
                }
            }
            out << '\n';

            if(mainrow == 0)
            {
                for(StringTable::Idx col = 0; col < number_of_cols; ++col)
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
print_table_grid(std::ostream& out, const StringTable& maintable)
{
    const std::vector<int> sizes = calc_all_column_widths(maintable, 0);

    // prefer enum since vs2017 requires one to capture constexpr (weird I know)
    enum
    {
        internal_space = 1
    };

    auto some_space = [&out](int spaces, char c = ' ') {
        for(int space_counter = 0; space_counter < spaces; space_counter += 1)
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

    for(StringTable::Idx y = 0; y < maintable.get_height(); ++y)
    {
        const auto subtable = split_table_cells_on_newline(maintable, y);
        for(StringTable::Idx suby = 0; suby < subtable.get_height(); suby += 1)
        {
            out << "|";
            for(StringTable::Idx x = 0; x < subtable.get_width(); ++x)
            {
                const auto cell = subtable[{x, suby}];
                some_space(internal_space);
                out << cell;
                some_space(sizes[x] - c_sizet_to_int(cell.length()));
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
