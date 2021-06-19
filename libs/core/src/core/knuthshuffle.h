#pragma once

#include <vector>

#include "core/random.h"
#include "core/cint.h"

namespace euphoria::core
{
    // Knuth shuffle aka Fisher–Yates shuffle
    // src: https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    // add one that respects the current item so it won't be the first one https://gamedev.stackexchange.com/a/29747/90313

    // todo(Gustav): add tests some how...?

    template <typename T>
    void
    knuth_shuffle(std::vector<T>* v, random* r)
    {
        for(int i = c_sizet_to_int(v->size()) - 1; i > 0; --i)
        {
            const auto j = get_random_in_range(r, i + 1); // 0 ≤ j ≤ i
            if(j != i)
            {
                std::swap((*v)[j], (*v)[i]);
            }
        }
    }

}
