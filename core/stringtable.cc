#include "core/stringtable.h"

#include <algorithm>

Table<std::string>
TableFromCsv(const std::string& data, char delim, char str)
{
  const auto AddRowToTable = [](Table<std::string>*             table,
                                const std::vector<std::string>& row) {
    if(row.empty())
    {
      return;
    }
    table->NewRow(row);
  };
  auto file = TextFileParser{data};

  Table<std::string> table;

  std::vector<std::string> row;
  std::stringstream        ss;
  bool                     inside_string = false;
  bool                     added         = false;
  while(file.HasMore())
  {
    const auto c = file.ReadChar();
    if(inside_string)
    {
      if(c == str)
      {
        if(file.PeekChar() == str)
        {
          added = true;
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
        added = true;
      }
    }
    else
    {
      if(c == str)
      {
        inside_string = true;
      }
      else if(c == delim)
      {
        row.push_back(ss.str());
        ss.str("");
        added = false;
      }
      else if(c == '\n')
      {
        if(added)
        {
          row.push_back(ss.str());
        }
        ss.str("");
        AddRowToTable(&table, row);
        row.resize(0);
        added = false;
      }
      else
      {
        ss << c;
        added = true;
      }
    }
  }

  if(added)
  {
    row.push_back(ss.str());
  }
  AddRowToTable(&table, row);

  return table;
}

int
ColumnWidth(const Table<std::string>& t, int c)
{
  int width = 0;
  for(size_t y=0; y<t.Height(); y+=1)
  {
    width = std::max<int>(width, t.Value(c, y).length());
  }
  return width;
}

std::vector<int>
ColumnWidths(const Table<std::string>& table, int extra)
{
  const auto       number_of_cols = table.Width();
  std::vector<int> sizes(number_of_cols);
  for(size_t i = 0; i < number_of_cols; ++i)
  {
    sizes[i] = ColumnWidth(table, i) + extra;
  }
  return sizes;
}

void
PrintTableSimple(std::ostream& out, const Table<std::string>& table)
{
  // todo: cleanup this function...
  const auto begin_str_padding = 1;
  const auto end_space_padding = 3;

  const auto       begin_str      = std::string(begin_str_padding, ' ');
  const auto       number_of_cols = table.Width();
  const auto       number_of_rows = table.Height();

  const std::vector<int> sizes = ColumnWidths(table, 1);

  const auto total_padding = begin_str.length() + end_space_padding;

  for(size_t row = 0; row < number_of_rows; ++row)
  {
    for(size_t col = 0; col < number_of_cols; ++col)
    {
      const auto cell = begin_str + table.Value(col, row);
      out << cell;
      if(col != number_of_cols - 1)
      {
        for(size_t i = cell.length(); i < sizes[col] + total_padding; ++i)
        {
          out << ' ';
        }
      }
    }
    out << '\n';

    if(row == 0)
    {
      for(size_t col = 0; col < number_of_cols; ++col)
      {
        const auto row_text = std::string(sizes[col] + begin_str_padding, '-') +
                              std::string(end_space_padding, ' ');
        out << row_text;
      }
      out << '\n';
    }
  }
}

void
PrintTableGrid(std::ostream& out, const Table<std::string>& table)
{
  const std::vector<int> sizes          = ColumnWidths(table, 0);

  constexpr auto internal_space = 1;

  auto some_space = [&out](int spaces, char c = ' ') {
    for(int i = 0; i < spaces; i += 1)
    {
      out << c;
    }
  };
  
  auto horizontal_line = [internal_space, &some_space, &out, &sizes]() {
    out << '+';
    for(auto s : sizes)
    {
      some_space(s + internal_space*2, '-');
      out << '+';
    }
    out << '\n';
  };

  horizontal_line();

  for(size_t y = 0; y < table.Height(); ++y)
  {
    out << "|";
    for(size_t x= 0; x < table.Width(); ++x)
    {
      const auto cell = table.Value(x, y);
      some_space(internal_space);
      out << cell;
      some_space(sizes[x] - cell.length());
      some_space(internal_space);
      out << '|';
    }
    out << '\n';

    if (y == 0)
    {
      horizontal_line();
    }
  }

  horizontal_line();
}
