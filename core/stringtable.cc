#include "core/stringtable.h"

#include "core/stringutils.h"

#include <algorithm>

namespace euphoria::core
{
    StringTable
    TableFromCsv(const std::string& csv_data, const CsvParserOptions& options)
    {
        const auto AddRowToTable
                = [](StringTable* table, const std::vector<std::string>& row) {
                      if(row.empty())
                      {
                          return;
                      }
                      table->NewRow(row);
                  };
        auto file = TextFileParser {csv_data};

        StringTable table;

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
                        // todo: generate error if string contains stuff other than whitespace
                        ss.str("");
                    }
                    inside_string   = true;
                    was_from_string = true;
                    added           = true;
                }
                else if(c == options.delim)
                {
                    auto column_data = ss.str();
                    if(!was_from_string && options.trim == CsvTrim::Trim)
                    {
                        column_data = Trim(column_data);
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
                        if(!was_from_string && options.trim == CsvTrim::Trim)
                        {
                            column_data = Trim(column_data);
                        }
                        row.push_back(column_data);
                    }
                    ss.str("");
                    AddRowToTable(&table, row);
                    row.resize(0);
                    added           = false;
                    was_from_string = false;
                }
                else
                {
                    if(was_from_string)
                    {
                        // skip
                        // todo: error on non whitespace?
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
            if(!was_from_string && options.trim == CsvTrim::Trim)
            {
                column_data = Trim(column_data);
            }
            row.push_back(column_data);
        }
        AddRowToTable(&table, row);

        return table;
    }

    int
    WidthOfString(const std::string& t)
    {
        // todo: bugfix longstring\nshort will return length of short, not longstring
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
    HeightOfString(const std::string& t)
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
    ColumnWidth(const StringTable& t, int c)
    {
        int width = 0;
        for(StringTable::I y = 0; y < t.Height(); y += 1)
        {
            width = std::max<int>(width, WidthOfString(t.Value(c, y)));
        }
        return width;
    }

    int
    RowHeight(const StringTable& t, int r)
    {
        int height = 0;
        for(StringTable::I x = 0; x < t.Width(); x += 1)
        {
            height = std::max<int>(height, HeightOfString(t.Value(x, r)));
        }
        return height;
    }

    std::vector<int>
    ColumnWidths(const StringTable& table, int extra)
    {
        const auto       number_of_cols = table.Width();
        std::vector<int> sizes(number_of_cols);
        for(StringTable::I i = 0; i < number_of_cols; ++i)
        {
            sizes[i] = ColumnWidth(table, i) + extra;
        }
        return sizes;
    }

    /// Return a new table based on row, where each cell is split at newline over many rows
    StringTable
    SplitTableCellsOnNewline(const StringTable& table, StringTable::I row)
    {
        auto ret = StringTable::FromWidthHeight(
                table.Width(), RowHeight(table, row));
        for(int c = 0; c < table.Width(); c += 1)
        {
            const auto rows = Split(table.Value(c, row), '\n');
            for(StringTable::I i = 0; i < StringTable::Ci(rows.size()); i += 1)
            {
                ret.Value(c, i, rows[i]);
            }
        }
        return ret;
    }

    void
    PrintTableSimple(std::ostream& out, const StringTable& maintable)
    {
        // todo: cleanup this function...
        const auto begin_str_padding = 1;
        const auto end_space_padding = 3;

        const auto begin_str      = std::string(begin_str_padding, ' ');
        const auto end_str        = std::string(end_space_padding, ' ');
        const auto number_of_cols = maintable.Width();
        const auto number_of_rows = maintable.Height();

        const std::vector<int> sizes = ColumnWidths(maintable, 1);

        const auto total_padding = begin_str_padding + end_space_padding;

        for(StringTable::I mainrow = 0; mainrow < number_of_rows; ++mainrow)
        {
            const auto subtable = SplitTableCellsOnNewline(maintable, mainrow);
            for(StringTable::I subrow = 0; subrow < subtable.Height(); ++subrow)
            {
                for(StringTable::I col = 0; col < number_of_cols; ++col)
                {
                    const auto cell = begin_str + subtable.Value(col, subrow);
                    int        line_length = cell.length();
                    out << cell;

                    if(col != number_of_cols - 1)
                    {
                        for(StringTable::I i = line_length;
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
                    for(StringTable::I col = 0; col < number_of_cols; ++col)
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
    PrintTableGrid(std::ostream& out, const StringTable& maintable)
    {
        const std::vector<int> sizes = ColumnWidths(maintable, 0);

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

        for(StringTable::I y = 0; y < maintable.Height(); ++y)
        {
            const auto subtable = SplitTableCellsOnNewline(maintable, y);
            for(StringTable::I suby = 0; suby < subtable.Height(); suby += 1)
            {
                out << "|";
                for(StringTable::I x = 0; x < subtable.Width(); ++x)
                {
                    const auto cell = subtable.Value(x, suby);
                    some_space(internal_space);
                    out << cell;
                    some_space(sizes[x] - cell.length());
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

}  // namespace euphoria::core
