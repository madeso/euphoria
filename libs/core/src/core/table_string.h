#pragma once




#include <iostream>
#include <functional>

#include "core/table.h"
#include "core/str.h"
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

        explicit TableGenerator(const std::vector<T>& d) : data(d) {}

        [[nodiscard]] StringTable
        to_table() const
        {
            StringTable ret;
            ret.new_row(column_titles);

            const auto s = column_titles.size();

            const auto indices = get_sorted_indices(data, *this);
            for(const auto index: indices)
            {
                const auto& d = data[index];
                std::vector<std::string> row_strings;
                row_strings.reserve(s);
                for(size_t i = 0; i < s; ++i)
                {
                    row_strings.emplace_back(column_to_string[i](d));
                }
                ret.new_row(row_strings);
            }

            return ret;
        }

        TableGenerator<T>&
        add_column(const std::string& title, ToStringFunction to_string)
        {
            column_titles.emplace_back(title);
            column_to_string.emplace_back(to_string);
            return *this;
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

    StringTable
    table_from_csv
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
    void
    print_table_simple(std::ostream& out, const StringTable& table);

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
    void
    print_table_grid(std::ostream& out, const StringTable& table);

}
