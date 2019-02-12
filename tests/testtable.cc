#include "core/table.h"

#include "catch.hpp"

TEST_CASE("table-generator", "[table]")
{
  struct Person { std::string first; std::string last; } ;

  const auto persons = std::vector<Person> {
    Person{"Jerry", "Seinfeld"},
    Person{"Elaine", "Benes"},
    Person{"Cosmo", "Kramer"},
    Person{"George", "Costanza"}
  };
  
  SECTION("first and last names")
  {
    const auto table = TableGenerator<Person>(persons)
      .Add("First name", [](const Person& p) -> std::string {return p.first;} )
      .Add("Last name", [](const Person& p) -> std::string {return p.last;} )
      .table;
    REQUIRE(table.size() == 2);
    const auto first = Column{"First name", "Jerry", "Elaine", "Cosmo", "George"};
    const auto last = Column{"Last name", "Seinfeld", "Benes", "Kramer", "Costanza"};
    CHECK(table[0]==first);
    CHECK(table[1]==last);
    // todo: add print tests
    // PrintTable(std::cout, table);
  }
}

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

  SECTION("string with quote")
  {
    const auto table = TableFromCsv("\"a \"\" b\"");
    const auto col = Column{"a \" b"};
    REQUIRE(table.size()==1);
    CHECK(table[0] == col);
  }
}

TEST_CASE("table-from_csv_not_default", "[table]")
{
  const auto firstcol = Column{"a", "1"};
  const auto firstcolstring = Column{"a b", "1"};
  const auto secondcol = Column{"b", "2"};

  const auto comma = 'c';
  const auto string = 's';

  SECTION("not ending with newline")
  {
    const auto table = TableFromCsv("acb\n1c2", comma, string);
    REQUIRE(table.size()==2);
    CHECK(table[0] == firstcol);
    CHECK(table[1] == secondcol);
  }

  SECTION("ending with newlines")
  {
    const auto table = TableFromCsv("acb\n\n\n1c2\n\n", comma, string);
    REQUIRE(table.size()==2);
    CHECK(table[0] == firstcol);
    CHECK(table[1] == secondcol);
  }

  SECTION("strings")
  {
    const auto table = TableFromCsv("sa bscb\n1c2", comma, string);
    REQUIRE(table.size()==2);
    CHECK(table[0] == firstcolstring);
    CHECK(table[1] == secondcol);
  }

  SECTION("not ending strings single col")
  {
    const auto table = TableFromCsv("sa b", comma, string);
    const auto errorcol = Column{"a b"};
    REQUIRE(table.size()==1);
    CHECK(table[0] == errorcol);
  }

  SECTION("not ending strings 2 cols")
  {
    const auto table = TableFromCsv("errcsa b", comma, string);
    const auto errorcol1 = Column{"err"};
    const auto errorcol2 = Column{"a b"};
    REQUIRE(table.size()==2);
    CHECK(table[0] == errorcol1);
    CHECK(table[1] == errorcol2);
  }

  SECTION("string with quote")
  {
    const auto table = TableFromCsv("sa ss bs", comma, string);
    const auto col = Column{"a s b"};
    REQUIRE(table.size()==1);
    CHECK(table[0] == col);
  }
}

