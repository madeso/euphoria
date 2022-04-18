#include "core/iterate.h"

#include "tests/approx_equal.h"

#include "catch.hpp"


namespace euco = euphoria::core;

TEST_CASE("iterate-sum-10", "[iterate]")
{
    int r = 0;
    for(auto i: euco::iterate(0, 10))
    {
        r += i+1;
    }
    REQUIRE(r == 55);
}

TEST_CASE("iterate-odd", "[iterate]")
{
    bool some_even = false;
    for(auto i: euco::iterate(1, 10, 2))
    {
        some_even = some_even || (i%2 == 0);
    }
    REQUIRE_FALSE(some_even);
}
