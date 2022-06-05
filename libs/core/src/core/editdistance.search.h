#pragma once


#include <queue>

#include <algorithm>

#include "core/editdistance.h"


namespace euphoria::core::search
{
    struct Match
    {
        std::string name;
        unsigned long changes;

        Match(const std::string& str, const std::string& input)
            : name(str)
            , changes(core::edit_distance(str, input))
        {
        }

        bool
        operator<(const Match& rhs) const
        {
            return changes < rhs.changes;
        }
    };

    template<typename T>
    struct Searcher
    {
        std::priority_queue<T> matches;

        void add(const T& t, std::size_t max_size)
        {
            matches.push(t);
            if(matches.size() > max_size)
            {
                matches.pop();
            }
        }
    };

    template<typename T, typename F, typename L>
    std::vector<T>
    find_closest(std::size_t max_size, const L& list, F&& f)
    {
        search::Searcher<T> searcher;
        for(auto entry: list)
        {
            searcher.add(f(entry), max_size);
        }
        std::vector<T> ret;

        while(!searcher.matches.empty())
        {
            ret.emplace_back(std::move(searcher.matches.top()));
            searcher.matches.pop();
        }

        std::reverse(ret.begin(), ret.end());

        return ret;
    }
}
