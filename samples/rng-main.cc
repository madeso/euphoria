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
    float sum = 0.0f;
    for(int i=0; i<count; i+=1)
    {
        const auto r = rng.Next();
        container.emplace_back(r);
        sum += r;
        if( i < small_count )
        {
            std::cout << r << " ";
        }
    }
    std::cout << "\n";

    const auto min = *std::min_element(container.begin(), container.end());
    const auto max = *std::max_element(container.begin(), container.end());
    std::cout << "min/max/avg: " << min << " " << max << " " << (sum/count) << "\n";
    std::cout << "\n";
}


struct Main
{
    int count = 100;
    int small_count = 10;
    Random random = Random{};

    template<typename TGenerator>
    void
    Print(const std::string& name)
    {
        PrintRandomNumbers<TGenerator>
        (
            name,
            &random,
            count,
            small_count
        );
    }

    void
    main()
    {
        Print<KnuthLcg>("knuth_lcg");
        Print<xorshift32>("xorshift32");
        Print<xorshift64>("xorshift64");
    }
};

int
main()
{
    auto m = Main{};
    m.main();
    return 0;
}

