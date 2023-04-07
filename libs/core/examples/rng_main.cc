#include "core/rng.h"

#include "core/random.h"

#include <algorithm>
#include <iostream>


#include <bitset>


using namespace euphoria;
using namespace euphoria::core;


template<typename TGenerator>
void
print_random_numbers
(
    const std::string& name,
    core::Random* random,
    int count,
    int small_count
)
{
    std::cout << name << ": ";

    auto rng = TGenerator{random->get_next_u32()};
    auto container = std::vector<float>{};
    float sum = 0.0f;
    for(int index=0; index<count; index+=1)
    {
        const auto r = rng.get_next_float01();
        container.emplace_back(r);
        sum += r;
        if( index < small_count )
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


struct Runner
{
    int count = 1000;
    int small_count = 10;
    core::Random rand = core::Random{};

    template<typename TGenerator>
    void
    print(const std::string& name)
    {
        print_random_numbers<TGenerator>
        (
            name,
            &rand,
            count,
            small_count
        );
    }

    template<int bits, typename TFunc>
    void
    print_ints(const std::string& name, int total_ints, TFunc f)
    {
        std::cout << name << ":\n";
        for(int index=0; index<total_ints; index+=1)
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
        print<RandomLnuthLcg>("knuth_lcg");
        print<RandomXorShift32>("xorshift32");
        print<RandomXorShift64>("xorshift64");

        const int int_count = 10;
        print_ints<32>("u32", int_count, [](core::Random* r) { return r->get_next_u32();});
        print_ints<64>("u64", int_count, [](core::Random* r) { return r->get_next_u64();});
    }
};

int
main()
{
    auto m = Runner{};
    m.main();
    return 0;
}

