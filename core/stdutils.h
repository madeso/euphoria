#ifndef EUPHORIA_STDUTILS_H
#define EUPHORIA_STDUTILS_H

#include "core/str.h"
#include "core/assert.h"

#include <map>
#include <vector>
#include <algorithm>

template <typename K, typename V>
std::vector<K>
Keys(const std::map<K, V>& m)
{
  std::vector<K> r;
  for(const auto& x : m)
  {
    r.push_back(x.first);
  }
  return r;
}

template <typename K>
std::vector<K>
Sorted(const std::vector<K>& k)
{
  auto s = k;
  std::sort(s.begin(), s.end());
  return s;
}

template <typename K, typename V>
std::vector<std::string>
KeysAsStrings(const std::map<K, V>& m)
{
  std::vector<std::string> strings;
  for(const auto& x : m)
  {
    strings.push_back(Str() << x.first);
  }
  return strings;
}

template <typename T>
bool
SwapBackAndEraseObject(T what, std::vector<T>* from)
{
  ASSERT(from);
  typedef std::vector<T> Vec;
  typename Vec::iterator result = std::find(from->begin(), from->end(), what);
  if(result == from->end())
  {
    return false;
  }

  typename Vec::iterator last = from->end();
  --last;  // point to a valid entry
  std::swap(*result, *last);
  from->pop_back();

  return true;
}

template <typename T, typename TFunc>
void
RemoveMatching(std::vector<T>* v, TFunc condition)
{
  v->erase(std::remove_if(v->begin(), v->end(), condition), v->end());
}

template <typename T, typename TFunc>
typename std::vector<T>::iterator
Search(std::vector<T>& v, TFunc condition)
{
  for(auto i = v.begin(); i != v.end(); i++)
  {
    if(condition(*i))
    {
      return i;
    }
  }

  return v.end();
}

#endif  // EUPHORIA_STDUTILS_H
