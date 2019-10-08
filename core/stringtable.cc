#include "core/stringtable.h"

#include "core/stringutils.h"

#include <algorithm>

namespace euphoria::core
{
    Table<std::string>
    TableFromCsv(const std::string& data, const CsvParserOptions& options)
    {
        const auto AddRowToTable = [](Table<std::string>*             table,
                                      const std::vector<std::string>& row) {
            if (row.empty())
            {
                return;
            }
            table->NewRow(row);
        };
        auto file = TextFileParser {data};

        Table<std::string> table;

        std::vector<std::string> row;
        std::stringstream        ss;
        bool                     inside_string   = false;
        bool                     added           = false;
        bool                     was_from_string = false;

        while (file.HasMore())
        {
            const auto c = file.ReadChar();
            if (inside_string)
            {
                if (c == options.str)
                {
                    if (file.PeekChar() == options.str)
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
                if (c == options.str)
                {
                    if (added)
                    {
                        // todo: generate error if string contains stuff other than whitespace
                        ss.str("");
                    }
                    inside_string   = true;
                    was_from_string = true;
                    added           = true;
                }
                else if (c == options.delim)
                {
                    auto data = ss.str();
                    if (!was_from_string && options.trim == CsvTrim::Trim)
                    {
                        data = Trim(data);
                    }
                    row.push_back(data);
                    ss.str("");
                    added           = false;
                    was_from_string = false;
                }
                else if (c == '\n')
                {
                    if (added)
                    {
                        auto data = ss.str();
                        if (!was_from_string && options.trim == CsvTrim::Trim)
                        {
                            data = Trim(data);
                        }
                        row.push_back(data);
                    }
                    ss.str("");
                    AddRowToTable(&table, row);
                    row.resize(0);
                    added           = false;
                    was_from_string = false;
                }
                else
                {
                    if (was_from_string)
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

        if (added)
        {
            auto data = ss.str();
            if (!was_from_string && options.trim == CsvTrim::Trim)
            {
                data = Trim(data);
            }
            row.push_back(data);
        }
        AddRowToTable(&table, row);

        return table;
    }

    int
    WidthOfString(const std::string& t)
    {
        // todo: bugfix longstring\nshort will return length of short, not longstring
        int w = 0;

        for (auto c: t)
        {
            if (c == '\n')
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
        for (auto c: t)
        {
            if (c == '\n')
            {
                h += 1;
            }
        }
        return h;
    }

    int
    ColumnWidth(const Table<std::string>& t, int c)
    {
        int width = 0;
        for (size_t y = 0; y < t.Height(); y += 1)
        {
            width = std::max<int>(width, WidthOfString(t.Value(c, y)));
        }
        return width;
    }

    int
    RowHeight(const Table<std::string>& t, int r)
    {
        int height = 0;
        for (size_t x = 0; x < t.Width(); x += 1)
        {
            height = std::max<int>(height, HeightOfString(t.Value(x, r)));
        }
        return height;
    }

    std::vector<int>
    ColumnWidths(const Table<std::string>& table, int extra)
    {
        const auto       number_of_cols = table.Width();
        std::vector<int> sizes(number_of_cols);
        for (size_t i = 0; i < number_of_cols; ++i)
        {
            sizes[i] = ColumnWidth(table, i) + extra;
        }
        return sizes;
    }

    /// Return a new table based on row, where each cell is split at newline over many rows
    Table<std::string>
    SplitTableCellsOnNewline(const Table<std::string>& table, size_t row)
    {
        auto ret = Table<std::string>::FromWidthHeight(
                table.Width(), RowHeight(table, row));
        for (int c = 0; c < table.Width(); c += 1)
        {
            const auto rows = Split(table.Value(c, row), '\n');
            for (int i = 0; i < rows.size(); i += 1)
            {
                ret.Value(c, i, rows[i]);
            }
        }
        return ret;
    }

    void
    PrintTableSimple(std::ostream& out, const Table<std::string>& maintable)
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

        for (size_t mainrow = 0; mainrow < number_of_rows; ++mainrow)
        {
            const auto subtable = SplitTableCellsOnNewline(maintable, mainrow);
            for (size_t subrow = 0; subrow < subtable.Height(); ++subrow)
            {
                for (size_t col = 0; col < number_of_cols; ++col)
                {
                    const auto cell = begin_str + subtable.Value(col, subrow);
                    int        line_length = cell.length();
                    out << cell;

                    if (col != number_of_cols - 1)
                    {
                        for (size_t i = line_length;
                             i < sizes[col] + total_padding;
                             ++i)
                        {
                            out << ' ';
                        }
                    }
                }
                out << '\n';

                if (mainrow == 0)
                {
                    for (size_t col = 0; col < number_of_cols; ++col)
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
    PrintTableGrid(std::ostream& out, const Table<std::string>& maintable)
    {
        const std::vector<int> sizes = ColumnWidths(maintable, 0);

        constexpr auto internal_space = 1;

        auto some_space = [&out](int spaces, char c = ' ') {
            for (int i = 0; i < spaces; i += 1)
            {
                out << c;
            }
        };

        auto horizontal_line = [internal_space, &some_space, &out, &sizes]() {
            out << '+';
            for (auto s: sizes)
            {
                some_space(s + internal_space * 2, '-');
                out << '+';
            }
            out << '\n';
        };

        horizontal_line();

        for (size_t y = 0; y < maintable.Height(); ++y)
        {
            const auto subtable = SplitTableCellsOnNewline(maintable, y);
            for (size_t suby = 0; suby < subtable.Height(); suby += 1)
            {
                out << "|";
                for (size_t x = 0; x < subtable.Width(); ++x)
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

            if (y == 0)
            {
                horizontal_line();
            }
        }

        horizontal_line();
    }

}  // namespace euphoria::core
