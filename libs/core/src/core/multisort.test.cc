#include "core/multisort.h"

#include "catch.hpp"

namespace euco = euphoria::core;

template <typename T>
struct SortBuilderCore : public euco::SortBuilder<T, SortBuilderCore<T>>
{};


namespace
{
    struct SortData
    {
        int i;
        int j;
    };
}


TEST_CASE("multisort-verify", "[multisort]")
{
    auto o = SortBuilderCore<SortData> {}
                     .sort(&SortData::i, euco::SortStyle::ascending)
                     .sort_order;

    const auto same = o[0]->sort(SortData {0, 0}, SortData {0, 0});
    const auto less = o[0]->sort(SortData {1, 0}, SortData {0, 0});
    const auto greater = o[0]->sort(SortData {0, 0}, SortData {1, 0});

    CHECK(same == 0);
    CHECK(less == -1);
    CHECK(greater == 1);
}

TEST_CASE("multisort-test", "[multisort]")
{
    // example usage

    const std::vector<SortData> data {
            {1, 1} /* index 0 */,
            {3, 2} /* index 1 */,
            {2, 2} /* index 2 */,
    };

    SECTION("basic sort asc")
    {
        auto si = get_sorted_indices(
                data,
                SortBuilderCore<SortData> {}.sort(&SortData::i, euco::SortStyle::ascending));
        const auto sorted = std::vector<size_t> {0, 2, 1};
        CHECK(si == sorted);
    }

    SECTION("basic sort desc")
    {
        auto si = get_sorted_indices(
                data,
                SortBuilderCore<SortData> {}.sort(&SortData::i, euco::SortStyle::descending));
        const auto sorted = std::vector<size_t> {1, 2, 0};
        CHECK(si == sorted);
    }

    SECTION("handle second")
    {
        auto si = get_sorted_indices(
                data,
                SortBuilderCore<SortData> {}
                        .sort(&SortData::j, euco::SortStyle::ascending)
                        .sort(&SortData::i, euco::SortStyle::descending));
        const auto sorted = std::vector<size_t> {0, 1, 2};
        CHECK(si == sorted);
    }
}
