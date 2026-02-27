#include "tests/equaler.h"

namespace eu::tests
{
    
#if 0
// mit from https://github.com/guilhermeagostinelli/levenshtein/blob/master/levenshtein.cpp
int levenshtein(const std::string& foo, const std::string& bar) {
    const auto foo_size = foo.size();
    const auto bar_size = bar.size();

    std::size_t state[foo_size + 1][bar_size + 1];
    
    if (foo_size == 0)
        return bar_size;
    if (bar_size == 0)
        return foo_size;

    for (std::size_t foo_index = 0; foo_index <= foo_size; foo_index++)
    {
        state[foo_index][0] = foo_index;
    }

    for (std::size_t bar_index = 0; bar_index <= bar_size; bar_index++)
    {
        state[0][bar_index] = bar_index;
    }

    for (std::size_t foo_index = 1; foo_index <= foo_size; foo_index++)
    {
        for (int bar_index = 1; bar_index <= bar_size; bar_index++)
        {
            const auto change_cost = (bar[bar_index - 1] == foo[foo_index - 1]) ? 0 : 1;

            const auto cost_foo = state[foo_index - 1][bar_index] + 1;
            const auto cost_bar = state[foo_index][bar_index - 1] + 1;
            const auto cost_both = state[foo_index - 1][bar_index - 1] + change_cost;

            const auto min_cost = std::min(std::min(cost_foo, cost_bar), cost_both);
            state[foo_index][bar_index] = min_cost;
        }
    }
    
    return state[foo_size][bar_size];
}

std::size_t find_index_of_first_diff(const std::string& first, const std::string& second) {
    std::size_t index = 0;
    for (; index <= std::min(first.length(), second.length()); index++)
    {
        if(first[index] != second[index]) return index;
    }
    return index;
}
#endif
}
