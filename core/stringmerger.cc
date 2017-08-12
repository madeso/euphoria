#include "core/stringmerger.h"

#include <string>
#include <sstream>
#include <vector>

#include "core/str.h"

std::string
StringMerger::Generate(const std::vector<std::string>& strings) const
{
  if(strings.empty())
  {
    return Str() << start_ << empty_ << end_;
  }

  std::ostringstream ss;

  ss << start_;

  const auto count = strings.size();
  for(std::vector<std::string>::size_type index = 0; index < count; ++index)
  {
    ss << strings[index];

    if(count != index + 1)
    {  // if this item isn't the last one in the list
      if(count == index + 2)
      {
        ss << final_separator_;
      }
      else
      {
        ss << separator_;
      }
    }
  }

  ss << end_;

  return ss.str();
}

const StringMerger&
StringMerger::EnglishAnd()
{
  static const StringMerger sep =
      StringMerger().set_separator(", ", " and ").SetStartEnd("", "");
  return sep;
}

const StringMerger&
StringMerger::EnglishOr()
{
  static const StringMerger sep =
      StringMerger().set_separator(", ", " or ").SetStartEnd("", "");
  return sep;
}

const StringMerger&
StringMerger::Array()
{
  static const StringMerger sep =
      StringMerger().set_separator(", ").SetStartEnd("[", "]");
  return sep;
}

StringMerger&
StringMerger::set_separator(const std::string& separator,
                            const std::string& final_separator)
{
  separator_       = separator;
  final_separator_ = final_separator;
  return *this;
}

StringMerger&
StringMerger::set_separator(const std::string& separator)
{
  separator_       = separator;
  final_separator_ = separator;
  return *this;
}

StringMerger&
StringMerger::set_empty(const std::string& empty)
{
  empty_ = empty;
  return *this;
}

StringMerger&
StringMerger::SetStartEnd(const std::string& start, const std::string& end)
{
  start_ = start;
  end_   = end;
  return *this;
}
