#include "core/tablelayout.h"

#include "catch.hpp"

namespace euco = euphoria::core;

template <typename T>
std::vector<T>
make_vec(std::initializer_list<T> args)
{
    std::vector<T> r {args};
    return r;
}

TEST_CASE("tablelayout-basic", "[tablelayout]")
{
    REQUIRE(euco::perform_table_layout<int>({3, 2}, 12) == make_vec({3, 2}));
}

TEST_CASE("tablelayout-one_expand", "[tablelayout]")
{
    REQUIRE(euco::perform_table_layout<int>({3, -1, 2}, 15) == make_vec({3, 10, 2}));
}

TEST_CASE("tablelayout-two_expand", "[tablelayout]")
{
    REQUIRE(euco::perform_table_layout<float>({-1, 3, 2, -1}, 15) == make_vec({5.0f, 3.0f, 2.0f, 5.0f}));
}

TEST_CASE("tablelayout-min_size", "[tablelayout]")
{
    REQUIRE(euco::perform_table_layout<int>({5, -1, 5}, 10) == make_vec({5, 0, 5}));
}

TEST_CASE("tablelayout-min_size_not_default", "[tablelayout]")
{
    REQUIRE(euco::perform_table_layout<int>({5, -1, 5}, 10, -10) == make_vec({5, -10, 5}));
}

TEST_CASE("tablelayout-smaller_size", "[tablelayout]")
{
    REQUIRE(euco::perform_table_layout<float>({5, -1, 5}, 5) == make_vec({2.5f, 0.0f, 2.5f}));
}

TEST_CASE("tablelayout-smaller_size_not_default", "[tablelayout]")
{
    REQUIRE(euco::perform_table_layout<float>({5, -1, 5}, 5, -2) == make_vec({2.5f, -2.0f, 2.5f}));
}
