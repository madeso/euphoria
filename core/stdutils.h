#ifndef EUPHORIA_STDUTILS_H
#define EUPHORIA_STDUTILS_H

#include "str.h"

#include <map>
#include <vector>

template <typename K, typename V>
std::vector<std::string>
Keys(const std::map<K, V>& m)
{
  std::vector<std::string> strings;
  for(const auto& x : m)
  {
    strings.push_back(Str() << x.first);
  }
  return strings;
}

#endif  // EUPHORIA_STDUTILS_H
