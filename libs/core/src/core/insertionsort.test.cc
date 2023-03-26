#include "core/insertionsort.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("insertionsort-empty", "[insertionsort]")
{
    const auto sorted = euco::get_insertion_sorted(std::vector<int> {});
    const auto expected = std::vector<int> {};
    CHECK(sorted == expected);
}

// quick sort is not stable, this will fail...
TEST_CASE("insertionsort-four-custom0", "[insertionsort]")
{
    const auto values = std::vector<int> {5, 3, 6, 1};
    const auto sorted = euco::get_insertion_sorted(values, [](int, int) { return 0; });
    const auto expected = values;
    CHECK(sorted == expected);
}

TEST_CASE("insertionsort-four", "[insertionsort]")
{
    const auto sorted = euco::get_insertion_sorted(std::vector<int> {5, 3, 6, 1});
    const auto expected = std::vector<int> {1, 3, 5, 6};
    CHECK(sorted == expected);
}

TEST_CASE("insertionsort-two", "[insertionsort]")
{
    const auto sorted = euco::get_insertion_sorted(std::vector<int> {8, 7});
    const auto expected = std::vector<int> {7, 8};
    CHECK(sorted == expected);
}

TEST_CASE("insertionsort-five", "[insertionsort]")
{
    const auto sorted = euco::get_insertion_sorted(std::vector<int> {5, 0, 1, 8, 7});
    const auto expected = std::vector<int> {0, 1, 5, 7, 8};
    CHECK(sorted == expected);
}

TEST_CASE("insertionsort-sorted", "[insertionsort]")
{
    const auto expected = std::vector<int> {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const auto sorted = euco::get_insertion_sorted(expected);
    CHECK(sorted == expected);
}

TEST_CASE("insertionsort-100", "[insertionsort]")
{
    const auto sorted = euco::get_insertion_sorted(std::vector<int> {
            95, 14, 6,  6,  90, 25, 42, 4,  12, 12, 97, 4,  1,  32, 55, 52, 48,
            19, 61, 85, 61, 42, 47, 34, 57, 47, 14, 70, 47, 14, 47, 97, 92, 6,
            20, 74, 4,  84, 30, 94, 16, 95, 73, 5,  90, 19, 2,  97, 73, 17, 27,
            34, 94, 54, 80, 65, 43, 82, 1,  55, 60, 78, 96, 43, 80, 36, 39, 54,
            76, 84, 74, 55, 14, 66, 80, 3,  3,  55, 97, 49, 98, 71, 40, 27, 55,
            3,  99, 58, 42, 48, 18, 47, 28, 5,  13, 12, 17, 29, 52, 96});
    const auto expected = std::vector<int> {
            1,  1,  2,  3,  3,  3,  4,  4,  4,  5,  5,  6,  6,  6,  12, 12, 12,
            13, 14, 14, 14, 14, 16, 17, 17, 18, 19, 19, 20, 25, 27, 27, 28, 29,
            30, 32, 34, 34, 36, 39, 40, 42, 42, 42, 43, 43, 47, 47, 47, 47, 47,
            48, 48, 49, 52, 52, 54, 54, 55, 55, 55, 55, 55, 57, 58, 60, 61, 61,
            65, 66, 70, 71, 73, 73, 74, 74, 76, 78, 80, 80, 80, 82, 84, 84, 85,
            90, 90, 92, 94, 94, 95, 95, 96, 96, 97, 97, 97, 97, 98, 99};
    CHECK(sorted == expected);
}

namespace
{
    struct TestData
    {
        std::string name;
        int i;

        bool
        operator<(const TestData& p) const
        {
            return name < p.name;
        }

        bool
        operator==(const TestData& p) const
        {
            return name == p.name && i == p.i;
        }
    };
    
    std::string to_string(const TestData& p)
        { return fmt::format("{}: {}", p.i, p.name); }
}

ADD_DEFAULT_FORMATTER(::TestData, std::string, to_string);


TEST_CASE("insertionsort-default_sort", "[insertionsort]")
{
    const auto dog = TestData {"dog", 3};
    const auto cat = TestData {"cat", 42};
    const auto human = TestData {"human", 1};
    const auto sorted = euco::get_insertion_sorted(std::vector<TestData> {dog, cat, human});
    const auto expected = std::vector<TestData> {cat, dog, human};
    CHECK(sorted == expected);
}

TEST_CASE("insertionsort-custom_sort", "[insertionsort]")
{
    const auto dog = TestData {"dog", 3};
    const auto cat = TestData {"cat", 42};
    const auto human = TestData {"human", 1};
    const auto sorted = euco::get_insertion_sorted
    (
        std::vector<TestData> {dog, cat, human},
        [](const TestData& lhs, const TestData& rhs)
        {
            return euco::default_compare_for_insertion_sort(lhs.i, rhs.i);
        }
    );
    const auto expected = std::vector<TestData> {human, dog, cat};
    CHECK(sorted == expected);
}


// todo(Gustav): add more tests
