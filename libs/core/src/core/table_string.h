#pragma once

#include <vector>
#include <string>
#include <ostream>
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
        using Converter = std::function<std::string(const T&)>;

        const std::vector<T>& data;
        std::vector<Converter> column_converter;
        std::vector<std::string> column_titles;

        explicit TableGenerator(const std::vector<T>& d) : data(d) {}

        [[nodiscard]] StringTable
        ToTable() const
        {
            StringTable ret;
            ret.NewRow(column_titles);

            const auto s = column_titles.size();

            const auto indices = GetSortedIndices(data, *this);
            for(const auto index: indices)
            {
                const auto& d = data[index];
                std::vector<std::string> row_strings;
                row_strings.reserve(s);
                for(size_t i = 0; i < s; ++i)
                {
                    row_strings.emplace_back(column_converter[i](d));
                }
                ret.NewRow(row_strings);
            }

            return ret;
        }

        TableGenerator<T>&
        AddColumn(const std::string& title, Converter converter)
        {
            column_titles.emplace_back(title);
            column_converter.emplace_back(converter);
            return *this;
        }
    };

    enum class CsvTrim
    {
        Trim,
        DontTrim
    };

    struct CsvParserOptions
    {
        char delim = ',';
        char str = '\"';
        CsvTrim trim = CsvTrim::DontTrim;
    };

    StringTable
    TableFromCsv(
            const std::string& data,
            const CsvParserOptions& options = CsvParserOptions());


    /*
        First name     Last name
        ------------   -----------
        Jerry          Seinfeld
        Elaine         Benes
        Cosmo          Kramer
        George         Costanza
        */
    void
    PrintTableSimple(std::ostream& out, const StringTable& table);

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
    PrintTableGrid(std::ostream& out, const StringTable& table);

}
