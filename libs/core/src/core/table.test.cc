#include "core/table.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("table-empty", "[table]")
{
    const auto t1 = euco::Table<int>::from_width_height(0, 0);
    CHECK(t1.get_width() == 0);
    CHECK(t1.get_height() == 0);

    const auto t2 = euco::Table<int> {};
    CHECK(t2.get_width() == 0);
    CHECK(t2.get_height() == 0);
}

TEST_CASE("table-size", "[table]")
{
    const auto t = euco::Table<int>::from_width_height(3, 2, 0);
    const auto same = euco::Table<int>::from_height_width(2, 3, 0);

    SECTION("test sizes")
    {
        CHECK(t.get_width() == 3);
        CHECK(same.get_width() == 3);

        CHECK(t.get_height() == 2);
        CHECK(same.get_height() == 2);
    }

    SECTION("verify data layout")
    {
        CHECK(t.get_data_index(0, 0) == 0);
        CHECK(t.get_data_index(2, 0) == 2);
        CHECK(t.get_data_index(0, 1) == 3);
    }

    CHECK(t(0, 0) == 0);
    CHECK(t(0, 0) == 0);
    CHECK(t(0, 0) == 0);
}

TEST_CASE("table-section_and_rows", "[table]")
{
    using T = euco::Table<std::string>;
    auto t = T::from_width_height(3, 2, "");
    for(T::Idx y = 0; y < t.get_height(); ++y)
    {
        for(T::Idx x = 0; x < t.get_width(); ++x)
        {
            t(x, y) = "{},{}"_format(x, y);
        }
    }

    SECTION("test getter")
    {
        CHECK(t(0, 0) == "0,0");
        CHECK(t(1, 0) == "1,0");
        CHECK(t(2, 0) == "2,0");

        CHECK(t(0, 1) == "0,1");
        CHECK(t(1, 1) == "1,1");
        CHECK(t(2, 1) == "2,1");
    }

    SECTION("test column getter")
    {
        const auto c0 = std::vector<std::string> {"0,0", "0,1"};
        CHECK(euco::calc_column_as_vector(t, 0) == c0);

        const auto c1 = std::vector<std::string> {"1,0", "1,1"};
        CHECK(euco::calc_column_as_vector(t, 1) == c1);

        const auto c2 = std::vector<std::string> {"2,0", "2,1"};
        CHECK(euco::calc_column_as_vector(t, 2) == c2);
    }

    SECTION("test row getter")
    {
        const auto r0 = std::vector<std::string> {"0,0", "1,0", "2,0"};
        CHECK(euco::calc_row_as_vector(t, 0) == r0);

        const auto r1 = std::vector<std::string> {"0,1", "1,1", "2,1"};
        CHECK(euco::calc_row_as_vector(t, 1) == r1);
    }
}

TEST_CASE("table-test_add", "[table]")
{
    auto t = euco::Table<int>();

    REQUIRE(0 == t.get_width());
    REQUIRE(0 == t.get_height());

    SECTION("basic usage")
    {
        t.add_row(std::vector<int> {1, 2, 3});

        REQUIRE(3 == t.get_width());
        REQUIRE(1 == t.get_height());

        t.add_row();
        t(0, 1) = 4;
        t(1, 1) = 5;
        t(2, 1) = 6;

        REQUIRE(3 == t.get_width());
        REQUIRE(2 == t.get_height());

        t.add_row(std::vector<int> {7, 8, 9});

        const auto r0 = std::vector<int> {1, 2, 3};
        CHECK(euco::calc_row_as_vector(t, 0) == r0);

        const auto r1 = std::vector<int> {4, 5, 6};
        CHECK(euco::calc_row_as_vector(t, 1) == r1);

        const auto r2 = std::vector<int> {7, 8, 9};
        CHECK(euco::calc_row_as_vector(t, 2) == r2);
    }

    SECTION("different array sizes")
    {
        const int d = -42;
        const auto r1d = std::vector<int> {1};
        const auto r10 = std::vector<int> {1, d};
        const auto r12 = std::vector<int> {1, 2};

        SECTION("1 then 2")
        {
            t.add_row(r1d, d);
            t.add_row(r12, d);
            REQUIRE(2 == t.get_width());
            REQUIRE(2 == t.get_height());
            CHECK(euco::calc_row_as_vector(t, 0) == r10);
            CHECK(euco::calc_row_as_vector(t, 1) == r12);
        }

        SECTION("2 then 1")
        {
            t.add_row(r12, d);
            t.add_row(r1d, d);
            REQUIRE(2 == t.get_width());
            REQUIRE(2 == t.get_height());
            CHECK(euco::calc_row_as_vector(t, 0) == r12);
            CHECK(euco::calc_row_as_vector(t, 1) == r10);
        }
    }
}
