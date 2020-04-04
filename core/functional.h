#ifndef CORE_FUNCTIONAL_H
#define CORE_FUNCTIONAL_H

#include <vector>
#include <utility>


namespace euphoria::core
{
    //
    // Common polymorphic higher order functions
    //

    // todo: add tests

    // combine 2 list into a list of pair
    template <typename A, typename B>
    std::vector<std::pair<A, B>>
    Zip(const std::vector<A>& as, const std::vector<B>& bs)
    {
        std::vector<std::pair<A, B>> ret;
        const auto s = std::min(as.size(), bs.size());
        for(size_t i = 0; i < s; i += 1)
        {
            ret.emplace_back(std::make_pair<A, B>(as[i], bs[i]));
        }
        return ret;
    }


    template <typename A, typename B>
    std::vector<std::pair<A, B>>
    ZipLongest
    (
        const std::vector<A>& as,
        const std::vector<B>& bs,
        A da = A(),
        B db = B()
    )
    {
        std::vector<std::pair<A, B>> ret;
        const auto s = std::max(as.size(), bs.size());
        for(size_t i = 0; i < s; i += 1)
        {
            const A a = i < as.size() ? as[i] : da;
            const B b = i < bs.size() ? bs[i] : db;
            ret.emplace_back(a, b);
        }
        return ret;
    }


    // map: A->B for a list, Select in c#
    template <typename F, typename T = F, typename C>
    std::vector<T>
    Map(const std::vector<F>& fs, C convert)
    {
        std::vector<T> r;
        r.reserve(fs.size());
        for(const auto f: fs)
        {
            r.emplace_back(convert(f));
        }
        return r;
    }


    template <typename T, typename K, typename V, typename C>
    std::vector<T>
    Map(const std::map<K, V>& fs, C convert)
    {
        std::vector<T> r;
        r.reserve(fs.size());
        for(const auto f: fs)
        {
            r.emplace_back(convert(f.first, f.second));
        }
        return r;
    }

    // concat map, SelectMany

    // filter - all values that satisfies a condition
    template <typename T, typename C>
    std::vector<T>
    Filter(const std::vector<T>& ts, C check)
    {
        std::vector<T> r;
        r.reserve(ts.size());
        for(const auto t: ts)
        {
            if(check(t))
            {
                r.emplace_back(t);
            }
        }
        return r;
    }

    // takewhile

    // return true if all matches
    template <typename T, typename C>
    bool
    All(const std::vector<T>& ts, C check)
    {
        std::vector<T> r;
        r.reserve(ts.size());
        for(const auto t: ts)
        {
            if(!check(t))
            {
                return false;
            }
        }
        return true;
    }

    // return true if any mathes
    template <typename T, typename C>
    bool
    Any(const std::vector<T>& ts, C check)
    {
        std::vector<T> r;
        r.reserve(ts.size());
        for(const auto t: ts)
        {
            if(check(t))
            {
                return true;
            }
        }
        return false;
    }

    // zipwith - zip + map - ZipWith in c#

    // iterate - create infinite iterator - useful?

    // foldr
    // foldl - Aggregate in c#
    // what do I have below?, what's the difference

    // combine list into single value
    template <typename T, typename R = T, typename C>
    R
    Fold(const std::vector<T>& ts, C concat, R zero)
    {
        R r = zero;
        for(const auto& t: ts)
        {
            r = concat(r, t);
        }
        return r;
    }

}  // namespace euphoria::core

#endif
