#pragma once

#include <map>
#include <algorithm>

#include "assert/assert.h"


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
    get_sorted(const std::vector<K>& k)
    {
        auto s = k;
        std::sort(s.begin(), s.end());
        return s;
    }

    template <typename K, typename V>
    std::vector<std::string>
    get_keys_as_strings(const std::map<K, V>& m)
    {
        std::vector<std::string> strings;
        for(const auto& x: m)
        {
            strings.push_back(fmt::format("{}", x.first));
        }
        return strings;
    }

    template <typename T>
    bool
    swap_back_and_erase_object(T what, std::vector<T>* from)
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
        --last; // point to a valid entry
        std::swap(*result, *last);
        from->pop_back();

        return true;
    }

    template <typename T, typename TFunc>
    void
    remove_matching(std::vector<T>* v, TFunc condition)
    {
        v->erase(std::remove_if(v->begin(), v->end(), condition), v->end());
    }

    template <typename T, typename TFunc>
    typename std::vector<T>::iterator
    find_first(std::vector<T>& v, TFunc condition)
    {
        for(auto iter = v.begin(); iter != v.end(); iter++)
        {
            if(condition(*iter))
            {
                return iter;
            }
        }

        return v.end();
    }


    // convert a enum class to it's underlying (int) type
    // src: https://twitter.com/idoccor/status/1314664849276899328
    template<typename E>
    constexpr typename std::underlying_type<E>::type
    base_cast(E e) noexcept
    {
        return static_cast<typename std::underlying_type<E>::type>(e);
    }

}

