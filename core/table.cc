#include "core/table.h"

Table TableFromCsv(const std::string& data, char delim, char str)
{
    const auto AddRowToTable = [](Table* table, const std::vector<std::string>& row)
    {
        if(table->empty())
        {
            for(const auto& c: row)
            {
                table->push_back(Column{c});
            }
        }
        else
        {
            // assert(row.size() == table->size());
            const auto columns = row.size();
            for(int i=0; i<columns; ++i)
            {
                const auto& c = row[i];
                (*table)[i].push_back(c);
            }
        }
    };
    auto file = TextFileParser{data};

    Table table;

    std::vector<std::string> row;
    std::stringstream ss;
    bool inside_string = false;
    while(file.HasMore())
    {
        const auto c = file.ReadChar();
        if(inside_string)
        {
            if(c == str)
            {
                if(file.PeekChar() == str)
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
            if( c == str)
            {
                inside_string = true;
            }
            else if( c == delim)
            {
                row.push_back(ss.str());
                ss.str("");
            }
            else if(c == '\n')
            {
                row.push_back(ss.str());
                ss.str("");
                AddRowToTable(&table, row);
                row.resize(0);
            }
            else
            {
                ss << c;
            }
        }
    }

    if(row.size() > 0)
    {
        row.push_back(ss.str());
        AddRowToTable(&table, row);
    }

    return table;
}

int ColumnWidth(const Column& col)
{
    int width = 0;
    for(const auto& t: col)
    {
        width = std::max<int>(width, t.length());
    }
    return width;
}

void PrintTable(std::ostream& out, const Table& table)
{
    const auto begin_str_padding = 1;
    const auto end_space_padding = 3;

    const auto begin_str = std::string(begin_str_padding, ' ');
    const auto number_of_cols = table.size();
    const auto number_of_rows = table[0].size();
    std::vector<int> sizes(number_of_cols);
    for(int i=0; i<number_of_cols; ++i) { sizes[i] = ColumnWidth(table[i]) + 1; }

    const auto total_padding = begin_str.length() + end_space_padding;

    for(int row=0; row<number_of_rows; ++row)
    {
        for(int col=0; col<number_of_cols; ++col)
        {
            const auto cell = begin_str + table[col][row];
            out << cell;
            if(col != number_of_cols-1 )
            {
                for(int i=cell.length(); i<sizes[col] + total_padding; ++i) { out << ' '; }
            }
        }
        out << '\n';

        if(row == 0)
        {
            for(int col=0; col<number_of_cols; ++col)
            {
                const auto row = std::string(sizes[col] + begin_str_padding, '-') + std::string(end_space_padding, ' ');
                out << row;
            }
            out << '\n';
        }
    }
}

