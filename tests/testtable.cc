#include "core/table.h"

#include "catch.hpp"

TEST_CASE("table-from_csv", "[table]")
{
  const auto table = TableFromCsv("a,b\n1,2");
  REQUIRE(table.size()==2);
  const auto firstcol = Column{"a", "1"};
  const auto secondcol = Column{"b", "2"};
  CHECK(table[0] == firstcol);
  CHECK(table[1] == secondcol);
}
