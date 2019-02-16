#include "core/multisort.h"

#include "catch.hpp"

namespace
{
  struct Data
  {
    int i;
    int j;
  };
}  // namespace

TEST_CASE("multisort-verify", "[multisort]")
{
  auto o = SortBuilder<Data>{}.Sort(&Data::i, SortStyle::Ascending).sort_order;

  const auto same    = o[0]->Sort(Data{0, 0}, Data{0, 0});
  const auto less    = o[0]->Sort(Data{1, 0}, Data{0, 0});
  const auto greater = o[0]->Sort(Data{0, 0}, Data{1, 0});

  CHECK(same == 0);
  CHECK(less == -1);
  CHECK(greater == 1);
}

TEST_CASE("multisort-test", "[multisort]")
{
  // example usage

  const std::vector<Data> data{
      {1, 1} /* index 0 */,
      {3, 2} /* index 1 */, 
      {2, 2} /* index 2 */,
  };

  SECTION("basic sort asc")
  {
    auto si = GetSortedIndices(
        data, SortBuilder<Data>{}.Sort(&Data::i, SortStyle::Ascending));
    const auto sorted = std::vector<size_t>{0, 2, 1};
    CHECK(si == sorted);
  }

  SECTION("basic sort desc")
  {
    auto si = GetSortedIndices(
        data, SortBuilder<Data>{}.Sort(&Data::i, SortStyle::Descending));
    const auto sorted = std::vector<size_t>{1, 2, 0};
    CHECK(si == sorted);
  }

  SECTION("handle second")
  {
    auto si = GetSortedIndices(
        data,
        SortBuilder<Data>{}
            .Sort(&Data::j, SortStyle::Ascending)
            .Sort(&Data::i, SortStyle::Descending)
            );
    const auto sorted = std::vector<size_t>{0, 1, 2};
    CHECK(si == sorted);
  }
}
