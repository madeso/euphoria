#include "core/table.h"
#include "core/str.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("table-empty", "[table]")
{
    const auto t1 = euco::Table<int>::FromWidthHeight(0, 0);
    CHECK(t1.GetWidth() == 0);
    CHECK(t1.GetHeight() == 0);

    const auto t2 = euco::Table<int> {};
    CHECK(t2.GetWidth() == 0);
    CHECK(t2.GetHeight() == 0);
}

TEST_CASE("table-size", "[table]")
{
    const auto t    = euco::Table<int>::FromWidthHeight(3, 2, 0);
    const auto same = euco::Table<int>::FromHeightWidth(2, 3, 0);

    SECTION("test sizes")
    {
        CHECK(t.GetWidth() == 3);
        CHECK(same.GetWidth() == 3);

        CHECK(t.GetHeight() == 2);
        CHECK(same.GetHeight() == 2);
    }

    SECTION("verify data layout")
    {
        CHECK(t.DataIndex(0, 0) == 0);
        CHECK(t.DataIndex(2, 0) == 2);
        CHECK(t.DataIndex(0, 1) == 3);
    }

    CHECK(t.Value(0, 0) == 0);
    CHECK(t.Value(0, 0) == 0);
    CHECK(t.Value(0, 0) == 0);
}

TEST_CASE("table-section_and_rows", "[table]")
{
    using T = euco::Table<std::string>;
    auto t  = T::FromWidthHeight(3, 2, "");
    for(T::I y = 0; y < t.GetHeight(); ++y)
        for(T::I x = 0; x < t.GetWidth(); ++x)
        {
            t.Value(x, y, euco::Str() << x << "," << y);
        }

    SECTION("test getter")
    {
        CHECK(t.Value(0, 0) == "0,0");
        CHECK(t.Value(1, 0) == "1,0");
        CHECK(t.Value(2, 0) == "2,0");

        CHECK(t.Value(0, 1) == "0,1");
        CHECK(t.Value(1, 1) == "1,1");
        CHECK(t.Value(2, 1) == "2,1");
    }

    SECTION("test column getter")
    {
        const auto c0 = std::vector<std::string> {"0,0", "0,1"};
        CHECK(euco::CalcColumnAsVector(t, 0) == c0);

        const auto c1 = std::vector<std::string> {"1,0", "1,1"};
        CHECK(euco::CalcColumnAsVector(t, 1) == c1);

        const auto c2 = std::vector<std::string> {"2,0", "2,1"};
        CHECK(euco::CalcColumnAsVector(t, 2) == c2);
    }

    SECTION("test row getter")
    {
        const auto r0 = std::vector<std::string> {"0,0", "1,0", "2,0"};
        CHECK(euco::CalcRowAsVector(t, 0) == r0);

        const auto r1 = std::vector<std::string> {"0,1", "1,1", "2,1"};
        CHECK(euco::CalcRowAsVector(t, 1) == r1);
    }
}

TEST_CASE("table-test_add", "[table]")
{
    auto t = euco::Table<int>();

    REQUIRE(0 == t.GetWidth());
    REQUIRE(0 == t.GetHeight());

    SECTION("basic usage")
    {
        t.NewRow(std::vector<int> {1, 2, 3});

        REQUIRE(3 == t.GetWidth());
        REQUIRE(1 == t.GetHeight());

        t.NewRow();
        t.Value(0, 1, 4);
        t.Value(1, 1, 5);
        t.Value(2, 1, 6);

        REQUIRE(3 == t.GetWidth());
        REQUIRE(2 == t.GetHeight());

        t.NewRow(std::vector<int> {7, 8, 9});

        const auto r0 = std::vector<int> {1, 2, 3};
        CHECK(euco::CalcRowAsVector(t, 0) == r0);

        const auto r1 = std::vector<int> {4, 5, 6};
        CHECK(euco::CalcRowAsVector(t, 1) == r1);

        const auto r2 = std::vector<int> {7, 8, 9};
        CHECK(euco::CalcRowAsVector(t, 2) == r2);
    }

    SECTION("different array sizes")
    {
        const int  d   = -42;
        const auto r1d = std::vector<int> {1};
        const auto r10 = std::vector<int> {1, d};
        const auto r12 = std::vector<int> {1, 2};

        SECTION("1 then 2")
        {
            t.NewRow(r1d, d);
            t.NewRow(r12, d);
            REQUIRE(2 == t.GetWidth());
            REQUIRE(2 == t.GetHeight());
            CHECK(euco::CalcRowAsVector(t, 0) == r10);
            CHECK(euco::CalcRowAsVector(t, 1) == r12);
        }

        SECTION("2 then 1")
        {
            t.NewRow(r12, d);
            t.NewRow(r1d, d);
            REQUIRE(2 == t.GetWidth());
            REQUIRE(2 == t.GetHeight());
            CHECK(euco::CalcRowAsVector(t, 0) == r12);
            CHECK(euco::CalcRowAsVector(t, 1) == r10);
        }
    }
}
