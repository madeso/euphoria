#include "core/rng.h"

#include <iostream>

int
main()
{
    using namespace euphoria::core;

    uint32_t max = 0;

    for(uint32_t i=0; true; i++)
    {
        auto rng = xorshift32{i};
        const auto v = rng.Nexti();
        if(v > max ) max = v;

        if( (i%1000000) == 0)
        {
            const auto m = std::numeric_limits<uint32_t>::max();
            std::cout
                << i
                << " / "
                << m
                << " = "
                << (static_cast<float>(i)/m)*100.0f
                << "\n";
        }
    }

    std::cout << "max is " << max << "\n";

    return 0;
}

