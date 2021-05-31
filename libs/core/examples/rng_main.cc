#include "core/rng.h"

#include "core/random.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>


using namespace euphoria;
using namespace euphoria::core;


template<typename TGenerator>
void
PrintRandomNumbers
(
    const std::string& name,
    core::random* random,
    int count,
    int small_count
)
{
    std::cout << name << ": ";

    auto rng = TGenerator{random->get_next_integer32()};
    auto container = std::vector<float>{};
    float sum = 0.0f;
    for(int i=0; i<count; i+=1)
    {
        const auto r = rng.get_next_float01();
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
    std::cout << "min/max/avg: " << min << " " << max << " " << (sum/static_cast<float>(count)) << "\n";
    std::cout << "\n";
}


struct Main
{
    int count = 1000;
    int small_count = 10;
    core::random rand = core::random{};

    template<typename TGenerator>
    void
    Print(const std::string& name)
    {
        PrintRandomNumbers<TGenerator>
        (
            name,
            &rand,
            count,
            small_count
        );
    }

    template<int bits, typename TFunc>
    void
    PrintInts(const std::string& name, int total_ints, TFunc f)
    {
        std::cout << name << ":\n";
        for(int i=0; i<total_ints; i+=1)
        {
            const auto t = f(&rand);
            const auto bs = std::bitset<bits>(t);
            std::cout << bs << "\n";
        }
        std::cout << "\n";
    }

    void
    main()
    {
        Print<random_knuth_lcg>("knuth_lcg");
        Print<random_xorshift32>("xorshift32");
        Print<random_xorshift64>("xorshift64");

        const int int_count = 10;
        PrintInts<32>("u32", int_count, [](core::random* r) { return r->get_next_integer32();});
        PrintInts<64>("u64", int_count, [](core::random* r) { return r->get_next_integer64();});
    }
};

int
main()
{
    auto m = Main{};
    m.main();
    return 0;
}

