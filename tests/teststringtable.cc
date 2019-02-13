#include "core/stringtable.h"

#include "catch.hpp"

TEST_CASE("stringtable-generator", "[stringtable]")
{
  struct Person
  {
    std::string first;
    std::string last;
  };

  const auto persons = std::vector<Person>{Person{"Jerry", "Seinfeld"},
                                           Person{"Elaine", "Benes"},
                                           Person{"Cosmo", "Kramer"},
                                           Person{"George", "Costanza"}};

  SECTION("first and last names")
  {
    const auto table =
        TableGenerator<Person>(persons)
            .Add(
                "First name",
                [](const Person& p) -> std::string { return p.first; })
            .Add(
                "Last name",
                [](const Person& p) -> std::string { return p.last; })
            .ToTable();
    REQUIRE(table.Width() == 2);
    REQUIRE(table.Height() == 5);
    const auto first =
        std::vector<std::string>{"First name", "Jerry", "Elaine", "Cosmo", "George"};
    const auto last = std::vector<std::string>{
        "Last name", "Seinfeld", "Benes", "Kramer", "Costanza"};
    CHECK(CalcColumnAsVector(table, 0) == first);
    CHECK(CalcColumnAsVector(table, 1) == last);
    // todo: add print tests
    // PrintTableSimple(std::cout, table);
  }
}

TEST_CASE("stringtable-from_csv_default", "[stringtable]")
{
  const auto firstcol       = std::vector<std::string>{"a", "1"};
  const auto firstcolstring = std::vector<std::string>{"a b", "1"};
  const auto secondcol      = std::vector<std::string>{"b", "2"};

  SECTION("not ending with newline")
  {
    const auto table = TableFromCsv("a,b\n1,2");
    REQUIRE(table.Width() == 2);
    REQUIRE(table.Height() == 2);
    CHECK(CalcColumnAsVector(table, 0) == firstcol);
    CHECK(CalcColumnAsVector(table, 1) == secondcol);
  }

  SECTION("ending with newlines")
  {
    const auto table = TableFromCsv("a,b\n\n\n1,2\n\n");
    REQUIRE(table.Width() == 2);
    REQUIRE(table.Height() == 2);
    CHECK(CalcColumnAsVector(table, 0) == firstcol);
    CHECK(CalcColumnAsVector(table, 1) == secondcol);
  }

  SECTION("strings")
  {
    const auto table = TableFromCsv("\"a b\",b\n1,2");
    REQUIRE(table.Width() == 2);
    REQUIRE(table.Height() == 2);
    CHECK(CalcColumnAsVector(table, 0) == firstcolstring);
    CHECK(CalcColumnAsVector(table, 1) == secondcol);
  }

  SECTION("not ending strings single col")
  {
    const auto table    = TableFromCsv("\"a b");
    const auto errorcol = std::vector<std::string>{"a b"};
    REQUIRE(table.Width() == 1);
    REQUIRE(table.Height() == 1);
    CHECK(CalcColumnAsVector(table, 0) == errorcol);
  }

  SECTION("not ending strings 2 cols")
  {
    const auto table     = TableFromCsv("err,\"a b");
    const auto errorcol1 = std::vector<std::string>{"err"};
    const auto errorcol2 = std::vector<std::string>{"a b"};
    REQUIRE(table.Width() == 2);
    REQUIRE(table.Height() == 1);
    CHECK(CalcColumnAsVector(table, 0) == errorcol1);
    CHECK(CalcColumnAsVector(table, 1) == errorcol2);
  }

  SECTION("string with quote")
  {
    const auto table = TableFromCsv("\"a \"\" b\"");
    const auto col   = std::vector<std::string>{"a \" b"};
    REQUIRE(table.Width() == 1);
    REQUIRE(table.Height() == 1);
    CHECK(CalcColumnAsVector(table, 0) == col);
  }
}

TEST_CASE("stringtable-from_csv_not_default", "[stringtable]")
{
  const auto firstcol       = std::vector<std::string>{"a", "1"};
  const auto firstcolstring = std::vector<std::string>{"a b", "1"};
  const auto secondcol      = std::vector<std::string>{"b", "2"};

  const auto comma  = 'c';
  const auto string = 's';

  SECTION("not ending with newline")
  {
    const auto table = TableFromCsv("acb\n1c2", comma, string);
    REQUIRE(table.Width() == 2);
    REQUIRE(table.Height() == 2);
    CHECK(CalcColumnAsVector(table, 0) == firstcol);
    CHECK(CalcColumnAsVector(table, 1) == secondcol);
  }

  SECTION("ending with newlines")
  {
    const auto table = TableFromCsv("acb\n\n\n1c2\n\n", comma, string);
    REQUIRE(table.Width() == 2);
    REQUIRE(table.Height() == 2);
    CHECK(CalcColumnAsVector(table, 0) == firstcol);
    CHECK(CalcColumnAsVector(table, 1) == secondcol);
  }

  SECTION("strings")
  {
    const auto table = TableFromCsv("sa bscb\n1c2", comma, string);
    REQUIRE(table.Width() == 2);
    REQUIRE(table.Height() == 2);
    CHECK(CalcColumnAsVector(table, 0) == firstcolstring);
    CHECK(CalcColumnAsVector(table, 1) == secondcol);
  }

  SECTION("not ending strings single col")
  {
    const auto table    = TableFromCsv("sa b", comma, string);
    const auto errorcol = std::vector<std::string>{"a b"};
    REQUIRE(table.Width() == 1);
    REQUIRE(table.Height() == 1);
    CHECK(CalcColumnAsVector(table, 0) == errorcol);
  }

  SECTION("not ending strings 2 cols")
  {
    const auto table     = TableFromCsv("errcsa b", comma, string);
    const auto errorcol1 = std::vector<std::string>{"err"};
    const auto errorcol2 = std::vector<std::string>{"a b"};
    REQUIRE(table.Width() == 2);
    REQUIRE(table.Height() == 1);
    CHECK(CalcColumnAsVector(table, 0) == errorcol1);
    CHECK(CalcColumnAsVector(table, 1) == errorcol2);
  }

  SECTION("string with quote")
  {
    const auto table = TableFromCsv("sa ss bs", comma, string);
    const auto col   = std::vector<std::string>{"a s b"};
    REQUIRE(table.Width() == 1);
    REQUIRE(table.Height() == 1);
    CHECK(CalcColumnAsVector(table, 0) == col);
  }
}
