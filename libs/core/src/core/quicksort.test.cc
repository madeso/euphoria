#include "core/quicksort.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("quicksort-empty", "[quicksort]")
{
    const auto sorted   = euco::QuickSort(std::vector<int> {});
    const auto expected = std::vector<int> {};
    CHECK(sorted == expected);
}

/*
// quick sort is not stable, this will fail...
TEST_CASE("quicksort-four-custom0", "[quicksort]")
{
  const auto values = std::vector<int>{5, 3, 6, 1};
  const auto sorted = QuickSort(values, [](int, int){return 0;});
  const auto expected = values;
  CHECK(sorted == expected);
}
*/

TEST_CASE("quicksort-four", "[quicksort]")
{
    const auto sorted   = euco::QuickSort(std::vector<int> {5, 3, 6, 1});
    const auto expected = std::vector<int> {1, 3, 5, 6};
    CHECK(sorted == expected);
}

TEST_CASE("quicksort-two", "[quicksort]")
{
    const auto sorted   = euco::QuickSort(std::vector<int> {8, 7});
    const auto expected = std::vector<int> {7, 8};
    CHECK(sorted == expected);
}

TEST_CASE("quicksort-five", "[quicksort]")
{
    const auto sorted   = euco::QuickSort(std::vector<int> {5, 0, 1, 8, 7});
    const auto expected = std::vector<int> {0, 1, 5, 7, 8};
    CHECK(sorted == expected);
}

TEST_CASE("quicksort-sorted", "[quicksort]")
{
    const auto expected = std::vector<int> {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const auto sorted   = euco::QuickSort(expected);
    CHECK(sorted == expected);
}

TEST_CASE("quicksort-100", "[quicksort]")
{
    const auto sorted   = euco::QuickSort(std::vector<int> {
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
    struct P
    {
        std::string name;
        int         i;

        bool
        operator<(const P& p) const
        {
            return name < p.name;
        }

        bool
        operator==(const P& p) const
        {
            return name == p.name && i == p.i;
        }
    };
    std::ostream&
    operator<<(std::ostream& o, const P& p)
    {
        o << p.i << ":" << p.name;
        return o;
    }
}  // namespace


TEST_CASE("quicksort-default_sort", "[quicksort]")
{
    const auto dog      = P {"dog", 3};
    const auto cat      = P {"cat", 42};
    const auto human    = P {"human", 1};
    const auto sorted   = euco::QuickSort(std::vector<P> {dog, cat, human});
    const auto expected = std::vector<P> {cat, dog, human};
    CHECK(sorted == expected);
}

TEST_CASE("quicksort-custom_sort", "[quicksort]")
{
    const auto dog    = P {"dog", 3};
    const auto cat    = P {"cat", 42};
    const auto human  = P {"human", 1};
    const auto sorted = euco::QuickSort(
            std::vector<P> {dog, cat, human}, [](const P& lhs, const P& rhs) {
                return euco::DefaultQuickSortFunction(lhs.i, rhs.i);
            });
    const auto expected = std::vector<P> {human, dog, cat};
    CHECK(sorted == expected);
}


// todo(Gustav): add more tests
