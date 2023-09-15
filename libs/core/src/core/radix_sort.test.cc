#include "tests/approx_equal.h"

#include "core/radix_sort.h"




#include "catch.hpp"

namespace euco = eu::core;

struct IntType
{
    static int
    get_id(int d)
    {
        return d;
    }
};

struct Dummy
{
    int id;
    std::string name;

    Dummy(int i, const std::string& n) : id(i), name(n) {}

    static int
    get_id(const Dummy& d)
    {
        return d.id;
    }

    bool
    operator==(const Dummy& rhs) const
    {
        return id == rhs.id && name == rhs.name;
    }
};

void
print_to(const Dummy& d, ::std::ostream* os)
{
    *os << d.id << ": " << d.name;
}

TEST_CASE("radix-bitshift_test", "[radix]")
{
    REQUIRE(euco::BucketBase16<int>::get_index(0, 0) == 0);
    REQUIRE(euco::BucketBase16<int>::get_index(16, 0) == 0);
    REQUIRE(euco::BucketBase16<int>::get_index(16, 1) == 1);
    REQUIRE(euco::BucketBase16<int>::get_index(4, 0) == 4);
    REQUIRE(euco::BucketBase16<int>::get_index(17, 0) == 1);
}

TEST_CASE("radix-values")
{
    const auto sorted1 = std::vector<Dummy>
    {
        {1, "dog"},
        {2, "cat"},
        {5, "pony"},
        {24, "bird"},
        {25, "horse"},
        {123, "fish"},
        {8390, "badger"}
    };
    const auto unsorted1 = std::vector<Dummy>
    {
        {25, "horse"},
        {24, "bird"},
        {5, "pony"},
        {1, "dog"},
        {8390, "badger"},
        {123, "fish"},
        {2, "cat"}
    };
    const auto sorted2 = std::vector<int>
    {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24, 25
    };
    const auto unsorted2 = std::vector<int>
    {
        5, 12, 13, 24, 4, 21, 8, 16, 6,
        20, 15, 25, 17, 3, 22, 14, 9,
        10, 18, 19, 2, 11, 23, 7, 1
    };


    SECTION("radix-already_sorted10", "[radix]")
    {
        std::vector<Dummy> d = sorted1;
        euco::do_inplace_radix_sort<Dummy, Dummy, euco::BucketBase10<int>, int>(&d);
        REQUIRE(d == sorted1);
    }

    SECTION("radix-not_sorted10", "[radix]")
    {
        std::vector<Dummy> d = unsorted1;
        euco::do_inplace_radix_sort<Dummy, Dummy, euco::BucketBase10<int>, int>(&d);
        REQUIRE(d == sorted1);
    }

    SECTION("radix-already_sorted16", "[radix]")
    {
        std::vector<Dummy> d = sorted1;
        euco::do_inplace_radix_sort<Dummy, Dummy, euco::BucketBase16<int>, int>(&d);
        REQUIRE(d == sorted1);
    }

    SECTION("radix-not_sorted16", "[radix]")
    {
        std::vector<Dummy> d = unsorted1;
        euco::do_inplace_radix_sort<Dummy, Dummy, euco::BucketBase16<int>, int>(&d);
        REQUIRE(d == sorted1);
    }

    ////////////////////////////////////////////////////////////////////////////////

    SECTION("radix-already_sorted10int", "[radix]")
    {
        std::vector<int> d = sorted2;
        euco::do_inplace_radix_sort<int, IntType, euco::BucketBase10<int>, int>(&d);
        REQUIRE(d == sorted2);
    }

    SECTION("radix-not_sorted10int", "[radix]")
    {
        std::vector<int> d = unsorted2;
        euco::do_inplace_radix_sort<int, IntType, euco::BucketBase10<int>, int>(&d);
        REQUIRE(d == sorted2);
    }

    SECTION("radix-already_sorted16int", "[radix]")
    {
        std::vector<int> d = sorted2;
        euco::do_inplace_radix_sort<int, IntType, euco::BucketBase16<int>, int>(&d);
        REQUIRE(d == sorted2);
    }

    SECTION("radix-not_sorted16int", "[radix]")
    {
        std::vector<int> d = unsorted2;
        euco::do_inplace_radix_sort<int, IntType, euco::BucketBase16<int>, int>(&d);
        REQUIRE(d == sorted2);
    }
}
