#include "core/iterate.h"

#include "tests/approx_equal.h"

#include "catch.hpp"


namespace core = eu::core;

template<typename T>
int sum_iterator(T iterator)
{
    int r = 0;
    for(auto i: iterator)
    {
        r += i+1;
    }
    return r;
}

TEST_CASE("iterate-sums", "[iterate]")
{
    REQUIRE(sum_iterator(core::iterate(0, 10)     ) == 55);
    REQUIRE(sum_iterator(core::iterate(0, 10, 3)  ) == 22);
    REQUIRE(sum_iterator(core::iterate(10, 0, -1) ) == 65);
    REQUIRE(sum_iterator(core::iterate(10, 0, -3) ) == 26);
}

TEST_CASE("iterate-odd", "[iterate]")
{
    bool some_even = false;
    for(auto i: core::iterate(1, 10, 2))
    {
        some_even = some_even || (i%2 == 0);
    }
    REQUIRE_FALSE(some_even);
}
