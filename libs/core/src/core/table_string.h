#pragma once




#include <iostream>
#include <functional>

#include "core/table.h"
#include "core/textfileparser.h"
#include "core/multisort.h"


namespace euphoria::core
{
    using StringTable = Table<std::string>;

    template <typename T>
    struct TableGenerator : public SortBuilder<T, TableGenerator<T>>
    {
        using ToStringFunction = std::function<std::string(const T&)>;

        const std::vector<T>& data;
        std::vector<ToStringFunction> column_to_string;
        std::vector<std::string> column_titles;

        explicit TableGenerator(const std::vector<T>& d)
            : data(d)
        {
        }

        TableGenerator<T>&
        add_column(const std::string& title, ToStringFunction to_string)
        {
            column_titles.emplace_back(title);
            column_to_string.emplace_back(to_string);
            return *this;
        }

        [[nodiscard]] StringTable
        to_table() const
        {
            StringTable ret;
            ret.add_row(column_titles);

            const auto column_count = column_titles.size();

            const auto indices = get_sorted_indices(data, *this);
            for(const auto index: indices)
            {
                const auto& d = data[index];
                std::vector<std::string> row_strings;
                row_strings.reserve(column_count);
                for(size_t column_index = 0; column_index < column_count; ++column_index)
                {
                    row_strings.emplace_back(column_to_string[column_index](d));
                }
                ret.add_row(row_strings);
            }

            return ret;
        }
    };

    enum class CsvTrim
    {
        trim,
        dont_trim
    };

    struct CsvParserOptions
    {
        char delim = ',';
        char str = '\"';
        CsvTrim trim = CsvTrim::dont_trim;
    };

    StringTable parse_csv
    (
        const std::string& data,
        const CsvParserOptions& options = CsvParserOptions()
    );


    /*
        First name     Last name
        ------------   -----------
        Jerry          Seinfeld
        Elaine         Benes
        Cosmo          Kramer
        George         Costanza
        */
    void print_table_simple(std::ostream& out, const StringTable& table);

    /*
        +------------+-----------+
        | First name | Last name |
        +------------+-----------+
        | Jerry      | Seinfeld  |
        | Elaine     | Benes     |
        | Cosmo      | Kramer    |
        | George     | Costanza  |
        +------------+-----------+
        */
    void print_table_grid(std::ostream& out, const StringTable& table);

}
