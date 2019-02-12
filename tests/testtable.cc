#include "core/table.h"

#include "catch.hpp"

TEST_CASE("table-from_csv", "[table]")
{
  const auto firstcol = Column{"a", "1"};
  const auto secondcol = Column{"b", "2"};

  SECTION("not ending with newline")
  {
    const auto table = TableFromCsv("a,b\n1,2");
    REQUIRE(table.size()==2);
    CHECK(table[0] == firstcol);
    CHECK(table[1] == secondcol);
  }

  SECTION("ending with newlines")
  {
    const auto table = TableFromCsv("a,b\n\n\n1,2\n\n");
    REQUIRE(table.size()==2);
    CHECK(table[0] == firstcol);
    CHECK(table[1] == secondcol);
  }
}

