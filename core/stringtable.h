#ifndef CORE_STRING_TABLE_H
#define CORE_STRING_TABLE_H

#include <vector>
#include <string>
#include <ostream>
#include <iostream>
#include <functional>

#include "core/table.h"
#include "core/str.h"
#include "core/textfileparser.h"

// generator

template <typename T>
struct TableGenerator
{
  using Converter = std::function<std::string(const T&)>;

  const std::vector<T>& data;
  std::vector<Converter> column_converter;
  std::vector<std::string> column_titles;
  
  explicit TableGenerator(const std::vector<T>& d)
      : data(d)
  {
  }

  Table<std::string>
  ToTable() const
  {
    Table<std::string> ret;
    ret.NewRow(column_titles);

    const auto s = column_titles.size();

    for(const auto& d : data)
    {
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
  Add(const std::string& title, Converter converter)
  {
    column_titles.emplace_back(title);
    column_converter.emplace_back(converter);
    return *this;
  }
};

Table<std::string>
TableFromCsv(const std::string& data, char delim = ',', char str = '\"');


/*
 First name     Last name
------------   -----------
 Jerry          Seinfeld
 Elaine         Benes
 Cosmo          Kramer
 George         Costanza
*/
void
PrintTableSimple(std::ostream& out, const Table<std::string>& table);

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
PrintTableGrid(std::ostream& out, const Table<std::string>& table);

#endif  // CORE_STRING_TABLE_H
