#ifndef CORE_KNUTH_SHUFFLE_H
#define CORE_KNUTH_SHUFFLE_H

#include <vector>

#include "core/random.h"

namespace euphoria::core
{
    // Knuth shuffle aka Fisher–Yates shuffle
    // src: https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    // add one that respects the current item so it won't be the first one https://gamedev.stackexchange.com/a/29747/90313

    // todo: add tests some how...?

    template <typename T>
    void
    KnuthShuffle(std::vector<T>* v, Random* r)
    {
        for(int i = v->size() - 1; i > 0; --i)
        {
            const auto j = r->NextRange(i + 1);  // 0 ≤ j ≤ i
            if(j != i)
            {
                std::swap((*v)[j], (*v)[i]);
            }
        }
    }

}  // namespace euphoria::core

#endif  // CORE_KNUTH_SHUFFLE_H
