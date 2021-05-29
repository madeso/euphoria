#pragma once

#include <string>
#include <queue>

#include "core/editdistance.h"


namespace euphoria::core::search
{
    struct match
    {
        std::string name;
        unsigned long changes;

        match(const std::string& str, const std::string& input)
            : name(str)
            , changes(core::edit_distance(str, input))
        {
        }

        bool
        operator<(const match& rhs) const
        {
            return changes < rhs.changes;
        }
    };

    template<typename T>
    struct searcher
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
    std::priority_queue<T>
    find_closest(std::size_t max_size, const L& list, F&& f)
    {
        search::searcher<T> searcher;
        for(auto entry: list)
        {
            searcher.add(f(entry), max_size);
        }
        return searcher.matches;
    }
}
