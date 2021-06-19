#include "core/multisort.h"

#include "catch.hpp"

namespace euco = euphoria::core;

template <typename T>
struct sort_builder_core : public euco::sort_builder<T, sort_builder_core<T>>
{};


namespace
{
    struct sort_data
    {
        int i;
        int j;
    };
}


TEST_CASE("multisort-verify", "[multisort]")
{
    auto o = sort_builder_core<sort_data> {}
                     .sort(&sort_data::i, euco::sort_style::ascending)
                     .sort_order;

    const auto same = o[0]->sort(sort_data {0, 0}, sort_data {0, 0});
    const auto less = o[0]->sort(sort_data {1, 0}, sort_data {0, 0});
    const auto greater = o[0]->sort(sort_data {0, 0}, sort_data {1, 0});

    CHECK(same == 0);
    CHECK(less == -1);
    CHECK(greater == 1);
}

TEST_CASE("multisort-test", "[multisort]")
{
    // example usage

    const std::vector<sort_data> data {
            {1, 1} /* index 0 */,
            {3, 2} /* index 1 */,
            {2, 2} /* index 2 */,
    };

    SECTION("basic sort asc")
    {
        auto si = get_sorted_indices(
                data,
                sort_builder_core<sort_data> {}.sort(&sort_data::i, euco::sort_style::ascending));
        const auto sorted = std::vector<size_t> {0, 2, 1};
        CHECK(si == sorted);
    }

    SECTION("basic sort desc")
    {
        auto si = get_sorted_indices(
                data,
                sort_builder_core<sort_data> {}.sort(&sort_data::i, euco::sort_style::descending));
        const auto sorted = std::vector<size_t> {1, 2, 0};
        CHECK(si == sorted);
    }

    SECTION("handle second")
    {
        auto si = get_sorted_indices(
                data,
                sort_builder_core<sort_data> {}
                        .sort(&sort_data::j, euco::sort_style::ascending)
                        .sort(&sort_data::i, euco::sort_style::descending));
        const auto sorted = std::vector<size_t> {0, 1, 2};
        CHECK(si == sorted);
    }
}
