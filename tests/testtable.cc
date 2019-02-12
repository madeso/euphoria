#include "core/table.h"

#include "catch.hpp"

TEST_CASE("table-from_csv_default", "[table]")
{
  const auto firstcol = Column{"a", "1"};
  const auto firstcolstring = Column{"a b", "1"};
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

  SECTION("strings")
  {
    const auto table = TableFromCsv("\"a b\",b\n1,2");
    REQUIRE(table.size()==2);
    CHECK(table[0] == firstcolstring);
    CHECK(table[1] == secondcol);
  }

  SECTION("not ending strings single col")
  {
    const auto table = TableFromCsv("\"a b");
    const auto errorcol = Column{"a b"};
    REQUIRE(table.size()==1);
    CHECK(table[0] == errorcol);
  }

  SECTION("not ending strings 2 cols")
  {
    const auto table = TableFromCsv("err,\"a b");
    const auto errorcol1 = Column{"err"};
    const auto errorcol2 = Column{"a b"};
    REQUIRE(table.size()==2);
    CHECK(table[0] == errorcol1);
    CHECK(table[1] == errorcol2);
  }
}

