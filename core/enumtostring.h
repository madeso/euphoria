#ifndef CORE_ENUM_TO_STRING_H
#define CORE_ENUM_TO_STRING_H

#include <string>
#include <map>
#include <vector>

#include "core/stringutils.h"
#include "core/levenshtein.h"

template<typename T>
struct MatchedEnum
{
  bool single_match = false;
  std::vector<T> enums;
};

template<typename T>
struct EnumToString
{
  std::map<T, std::string> enum_to_string;
  std::map<std::string, T> string_to_enum;

  // todo: replace with initializer list?
  EnumToString<T>& Add(const std::string& name, T t)
  {
    enum_to_string.insert(std::make_pair(t, name));
    string_to_enum.insert(std::make_pair(ToLower(name), t));
    return *this;
  }

  std::string ToString(T t) const
  {
    auto found = enum_to_string(t);
    if(found != enum_to_string.end())
    {
      return found->second;
    }
    ASSERT("Enum no added");
    return "???";
  }

  MatchedEnum<T> Match(const std::string& input, size_t max_size = 5) const
  {
    auto found = string_to_enum.find(ToLower(str));
    if(found != string_to_enum.end())
    {
      return MatchedEnum<T>{true, {found->second}};
    }
    struct Match { T t; unsigned long changes;
      bool operator<(const Match& rhs) {return changes<rhs.changes;} };
    std::priority_queue<Match> matches;
    for(auto entry: enum_to_string)
    {
      const auto t = entry.first;
      const auto str = entry.second;
      const auto changes = LevenshteinDistance(str, input);
      matches.push({t, changes});
      if(matches.size() > max_size)
      {
        matches.pop();
      }
    }
    auto ret = MatchedEnum<T>{};
    while(!matches.empty())
    {
      ret.enums.push_back(matches.pop().t);
    }
    return ret;
  }
};

template<typename T>
const EnumToString<T>&
GetEnumToString();
// no implmenentation: only here for template specialization

#define BEGIN_ENUM_LIST(T) template<> const ::EnumToString<T>& ::GetEnumToString<T>() { static const auto r = ::EnumToString<T>{}
#define ENUM_VALUE(T, V) .Add(#V, T::V)
#define END_ENUM_LIST() ; return r; }

#endif  // CORE_ENUM_TO_STRING_H

