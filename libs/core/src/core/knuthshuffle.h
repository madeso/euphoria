#pragma once



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
    shuffle(std::vector<T>* v, Random* r)
    {
        for(int index = c_sizet_to_int(v->size()) - 1; index > 0; index -= 1)
        {
            const auto j = get_random_in_range(r, index + 1); // 0 ≤ j ≤ index
            if(j != index)
            {
                std::swap((*v)[j], (*v)[index]);
            }
        }
    }

}
