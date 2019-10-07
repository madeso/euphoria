#include "core/stringtable.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("stringtable-generator", "[stringtable]")
{
    struct Person
    {
        std::string first;
        std::string last;
    };

    const auto persons = std::vector<Person> {Person {"Jerry", "Seinfeld"},
                                              Person {"Elaine", "Benes"},
                                              Person {"Cosmo", "Kramer"},
                                              Person {"George", "Costanza"}};

    SECTION("first and last names")
    {
        const auto table = euco::TableGenerator<Person>(persons)
                                   .AddColumn(
                                           "First name",
                                           [](const Person& p) -> std::string {
                                               return p.first;
                                           })
                                   .AddColumn(
                                           "Last name",
                                           [](const Person& p) -> std::string {
                                               return p.last;
                                           })
                                   .ToTable();
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 5);
        const auto first = std::vector<std::string> {
                "First name", "Jerry", "Elaine", "Cosmo", "George"};
        const auto last = std::vector<std::string> {
                "Last name", "Seinfeld", "Benes", "Kramer", "Costanza"};
        CHECK(euco::CalcColumnAsVector(table, 0) == first);
        CHECK(euco::CalcColumnAsVector(table, 1) == last);
        // todo: add print tests
        // PrintTableSimple(std::cout, table);
    }

    SECTION("sort on last name")
    {
        const auto table = euco::TableGenerator<Person>(persons)
                                   .AddColumn(
                                           "First name",
                                           [](const Person& p) -> std::string {
                                               return p.first;
                                           })
                                   .AddColumn(
                                           "Last name",
                                           [](const Person& p) -> std::string {
                                               return p.last;
                                           })
                                   .Sort(&Person::last)
                                   .ToTable();
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 5);
        const auto first = std::vector<std::string> {
                "First name", "Elaine", "George", "Cosmo", "Jerry"};
        const auto last = std::vector<std::string> {
                "Last name", "Benes", "Costanza", "Kramer", "Seinfeld"};
        CHECK(euco::CalcColumnAsVector(table, 0) == first);
        CHECK(euco::CalcColumnAsVector(table, 1) == last);
        // PrintTableGrid(std::cout, table);
    }
}

TEST_CASE("stringtable-from_csv_default", "[stringtable]")
{
    const auto firstcol       = std::vector<std::string> {"a", "1"};
    const auto firstcolstring = std::vector<std::string> {"a b", "1"};
    const auto secondcol      = std::vector<std::string> {"b", "2"};
    const auto secondcolspace      = std::vector<std::string> {" b ", "2"};

    SECTION("not ending with newline")
    {
        const auto table = euco::TableFromCsv("a,b\n1,2");
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 2);
        CHECK(euco::CalcColumnAsVector(table, 0) == firstcol);
        CHECK(euco::CalcColumnAsVector(table, 1) == secondcol);
    }

    SECTION("ending with newlines")
    {
        const auto table = euco::TableFromCsv("a,b\n\n\n1,2\n\n");
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 2);
        CHECK(euco::CalcColumnAsVector(table, 0) == firstcol);
        CHECK(euco::CalcColumnAsVector(table, 1) == secondcol);
    }

    SECTION("with spaces (trim)")
    {
        euco::CsvParserOptions options;
        options.trim = euco::CsvTrim::Trim;

        const auto table = euco::TableFromCsv("  a  ,  b  \n  1    ,  2  ", options);
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 2);
        CHECK(euco::CalcColumnAsVector(table, 0) == firstcol);
        CHECK(euco::CalcColumnAsVector(table, 1) == secondcol);
    }

    SECTION("with spaces (keep)")
    {
        const auto table = euco::TableFromCsv("a, b \n1,2");
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 2);
        CHECK(euco::CalcColumnAsVector(table, 0) == firstcol);
        CHECK(euco::CalcColumnAsVector(table, 1) == secondcolspace);
    }

    SECTION("with spaces (string)")
    {
        euco::CsvParserOptions options;
        options.trim = euco::CsvTrim::Trim;

        const auto table = euco::TableFromCsv("a,\" b \"\n1,2", options);
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 2);
        CHECK(euco::CalcColumnAsVector(table, 0) == firstcol);
        CHECK(euco::CalcColumnAsVector(table, 1) == secondcolspace);
    }

    SECTION("strings")
    {
        const auto table = euco::TableFromCsv("\"a b\",b\n1,2");
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 2);
        CHECK(euco::CalcColumnAsVector(table, 0) == firstcolstring);
        CHECK(euco::CalcColumnAsVector(table, 1) == secondcol);
    }

    SECTION("not ending strings single col")
    {
        const auto table    = euco::TableFromCsv("\"a b");
        const auto errorcol = std::vector<std::string> {"a b"};
        REQUIRE(table.Width() == 1);
        REQUIRE(table.Height() == 1);
        CHECK(euco::CalcColumnAsVector(table, 0) == errorcol);
    }

    SECTION("not ending strings 2 cols")
    {
        const auto table     = euco::TableFromCsv("err,\"a b");
        const auto errorcol1 = std::vector<std::string> {"err"};
        const auto errorcol2 = std::vector<std::string> {"a b"};
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 1);
        CHECK(euco::CalcColumnAsVector(table, 0) == errorcol1);
        CHECK(euco::CalcColumnAsVector(table, 1) == errorcol2);
    }

    SECTION("string with quote")
    {
        const auto table = euco::TableFromCsv("\"a \"\" b\"");
        const auto col   = std::vector<std::string> {"a \" b"};
        REQUIRE(table.Width() == 1);
        REQUIRE(table.Height() == 1);
        CHECK(euco::CalcColumnAsVector(table, 0) == col);
    }
}

TEST_CASE("stringtable-from_csv_not_default", "[stringtable]")
{
    const auto firstcol       = std::vector<std::string> {"a", "1"};
    const auto firstcolstring = std::vector<std::string> {"a b", "1"};
    const auto secondcol      = std::vector<std::string> {"b", "2"};

    euco::CsvParserOptions options;
    options.delim  = 'c';
    options.str = 's';

    SECTION("not ending with newline")
    {
        const auto table = euco::TableFromCsv("acb\n1c2", options);
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 2);
        CHECK(euco::CalcColumnAsVector(table, 0) == firstcol);
        CHECK(euco::CalcColumnAsVector(table, 1) == secondcol);
    }

    SECTION("ending with newlines")
    {
        const auto table
                = euco::TableFromCsv("acb\n\n\n1c2\n\n", options);
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 2);
        CHECK(euco::CalcColumnAsVector(table, 0) == firstcol);
        CHECK(euco::CalcColumnAsVector(table, 1) == secondcol);
    }

    SECTION("strings")
    {
        const auto table = euco::TableFromCsv("sa bscb\n1c2", options);
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 2);
        CHECK(euco::CalcColumnAsVector(table, 0) == firstcolstring);
        CHECK(euco::CalcColumnAsVector(table, 1) == secondcol);
    }

    SECTION("not ending strings single col")
    {
        const auto table    = euco::TableFromCsv("sa b", options);
        const auto errorcol = std::vector<std::string> {"a b"};
        REQUIRE(table.Width() == 1);
        REQUIRE(table.Height() == 1);
        CHECK(euco::CalcColumnAsVector(table, 0) == errorcol);
    }

    SECTION("not ending strings 2 cols")
    {
        const auto table     = euco::TableFromCsv("errcsa b", options);
        const auto errorcol1 = std::vector<std::string> {"err"};
        const auto errorcol2 = std::vector<std::string> {"a b"};
        REQUIRE(table.Width() == 2);
        REQUIRE(table.Height() == 1);
        CHECK(euco::CalcColumnAsVector(table, 0) == errorcol1);
        CHECK(euco::CalcColumnAsVector(table, 1) == errorcol2);
    }

    SECTION("string with quote")
    {
        const auto table = euco::TableFromCsv("sa ss bs", options);
        const auto col   = std::vector<std::string> {"a s b"};
        REQUIRE(table.Width() == 1);
        REQUIRE(table.Height() == 1);
        CHECK(euco::CalcColumnAsVector(table, 0) == col);
    }
}
