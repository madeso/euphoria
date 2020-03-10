#include "core/rng.h"

#include "core/random.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>


using namespace euphoria::core;


template<typename TGenerator>
void
PrintRandomNumbers
(
    const std::string& name,
    Random* random,
    int count,
    int small_count
)
{
    std::cout << name << ": ";

    auto rng = TGenerator{random->NextInteger()};
    auto container = std::vector<float>{};
    for(int i=0; i<count; i+=1)
    {
        const auto r = rng.Next();
        container.emplace_back(r);
        if( i < small_count )
        {
            std::cout << r << " ";
        }
    }
    std::cout << "\n";

    const auto min = *std::min_element(container.begin(), container.end());
    const auto max = *std::max_element(container.begin(), container.end());
    std::cout << "min/max: " << min << " " << max << "\n";
    std::cout << "\n";
}


int
main()
{
    constexpr auto count = 100;
    constexpr auto small_count = 10;
    auto random = Random{};

    PrintRandomNumbers<wyhash64>
    (
        "wyhash64",
        &random,
        count,
        small_count
    );
    return 0;
}

