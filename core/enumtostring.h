#ifndef CORE_ENUM_TO_STRING_H
#define CORE_ENUM_TO_STRING_H

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <ostream>

#include "core/assert.h"
#include "core/stringutils.h"
#include "core/editdistance.h"

namespace euphoria::core
{
  template<typename T>
  struct MatchedEnum
  {
    bool single_match = false;
    std::vector<T> values;
  };

  template<typename T>
  struct EnumToStringImpl
  {
    std::map<T, std::string> enum_to_string;
    std::map<std::string, T> string_to_enum;

    // todo: replace with initializer list?
    EnumToStringImpl<T>& Add(const std::string& name, T t)
    {
      enum_to_string.insert(std::make_pair(t, name));
      string_to_enum.insert(std::make_pair(ToLower(name), t));
      return *this;
    }

    std::string ToString(T t) const
    {
      auto found = enum_to_string.find(t);
      if(found != enum_to_string.end())
      {
        return found->second;
      }
      DIE("Enum not added");
      return "???";
    }

    std::vector<std::string> ListNames() const
    {
      std::vector<std::string> ret;
      for(auto entry: enum_to_string)
      {
        ret.emplace_back(entry.second);
      }
      return ret;
    }

    std::vector<T> ListValues() const
    {
      std::vector<T> ret;
      for(auto entry: enum_to_string)
      {
        ret.emplace_back(entry.first);
      }
      return ret;
    }

    MatchedEnum<T> Match(const std::string& input, size_t max_size) const
    {
      auto found = string_to_enum.find(ToLower(input));
      if(found != string_to_enum.end())
      {
        return MatchedEnum<T>{true, {found->second}};
      }
      struct Match { T t; unsigned long changes;
        bool operator<(const Match& rhs) const {return changes<rhs.changes;} };
      std::priority_queue<Match> matches;
      for(auto entry: enum_to_string)
      {
        const auto t = entry.first;
        const auto str = entry.second;
        const auto changes = EditDistance(str, input);
        matches.push({t, changes});
        if(matches.size() > max_size)
        {
          matches.pop();
        }
      }
      auto ret = MatchedEnum<T>{};
      while(!matches.empty())
      {
        ret.values.insert(ret.values.begin(), matches.top().t);
        matches.pop();
      }
      return ret;
    }
  };

  template<typename T>
  struct GetEnumToString
  {
    enum { IsDefined = 0 };

    // using Type = T Type;

    // no implmenentation: only here for template specialization
    static const EnumToStringImpl<T>& EnumValues();
  };


  template<typename T>
  MatchedEnum<T> StringToEnum(const std::string& input, size_t max_size = 5)
  {
    return GetEnumToString<T>::EnumValues().Match(input, max_size);
  }

  template<typename T>
  std::string EnumToString(T t)
  {
    return GetEnumToString<T>::EnumValues().ToString(t);
  }

  template<typename T>
  std::vector<std::string> EnumToString(const std::vector<T>& ts)
  {
    std::vector<std::string> ret;
    for(auto t: ts)
    {
      ret.emplace_back(GetEnumToString<T>::EnumValues().ToString(t));
    }
    return ret;
  }

  template<typename T>
  std::vector<std::string> EnumToString()
  {
    return GetEnumToString<T>::EnumValues().ListNames();
  }

  template<typename T>
  std::vector<T> EnumValues()
  {
    return GetEnumToString<T>::EnumValues().ListValues();
  }
}

template <typename T>
typename std::enable_if_t<euphoria::core::GetEnumToString<T>::IsDefined==1, std::ostream>& 
operator<<( std::ostream& os, T const& value)
{
  os << euphoria::core::EnumToString<T>( value );
  return os;
}

#define DECLARE_ENUM_LIST(T)\
  template<> struct euphoria::core::GetEnumToString<T>\
  {\
    using Type = T;\
    enum { IsDefined = 1 };\
    static const euphoria::core::EnumToStringImpl<T>& EnumValues();\
  };

#define BEGIN_ENUM_LIST(T)\
  const euphoria::core::EnumToStringImpl<T>& euphoria::core::GetEnumToString<T>::EnumValues()\
  {\
  static const auto r = euphoria::core::EnumToStringImpl<T>{}
#define ENUM_VALUE(T, V)                 .Add(#V, T::V)
#define END_ENUM_LIST()\
    ;\
    return r;\
  }

// todo: add a foreach macro call?
// https://codecraft.co/2014/11/25/variadic-macros-tricks/

#endif  // CORE_ENUM_TO_STRING_H

