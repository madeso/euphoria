#ifndef CORE_STRING_TABLE_H
#define CORE_STRING_TABLE_H

#include <vector>
#include <string>
#include <ostream>
#include <iostream>

#include "core/str.h"
#include "core/textfileparser.h"

// first string is title row
using Column      = std::vector<std::string>;
using StringTable = std::vector<Column>;

// generator

template <typename T>
struct TableGenerator
{
  StringTable           table;
  const std::vector<T>& data;

  explicit TableGenerator(const std::vector<T>& d)
      : data(d)
  {
  }

  template <typename Func>
  TableGenerator<T>&
  Add(const std::string& title, Func func)
  {
    Column col;
    col.reserve(data.size() + 1);
    col.emplace_back(title);
    for(const auto& d : data)
    {
      col.push_back(func(d));
    }
    table.emplace_back(col);
    return *this;
  }
};

StringTable
TableFromCsv(const std::string& data, char delim = ',', char str = '\"');

void
PrintTable(std::ostream& out, const StringTable& table);

#endif  // CORE_STRING_TABLE_H
