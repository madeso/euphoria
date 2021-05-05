#ifndef EUPHORIA_STDUTILS_H
#define EUPHORIA_STDUTILS_H

#include "core/str.h"
#include "core/assert.h"

#include <map>
#include <vector>
#include <algorithm>

namespace euphoria::core
{
    template <typename K, typename V>
    std::vector<K>
    get_keys(const std::map<K, V>& m)
    {
        std::vector<K> r;
        for(const auto& x: m)
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
        for(const auto& x: m)
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
        using Vec = std::vector<T>;
        typename Vec::iterator result
                = std::find(from->begin(), from->end(), what);
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


    // convert a enum class to it's underlying (int) type
    // src: https://twitter.com/idoccor/status/1314664849276899328
    template<typename E>
    constexpr typename std::underlying_type<E>::type ToBase(E e) noexcept
    {
        return static_cast<typename std::underlying_type<E>::type>(e);
    }

}  // namespace euphoria::core

#endif  // EUPHORIA_STDUTILS_H
