#include "core/table_string.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("stringtable-generator", "[stringtable]")
{
    struct person
    {
        std::string first;
        std::string last;
    };

    const auto persons = std::vector<person>
    {
        person {"Jerry", "Seinfeld"},
        person {"Elaine", "Benes"},
        person {"Cosmo", "Kramer"},
        person {"George", "Costanza"}
    };

    SECTION("first and last names")
    {
        const auto table = euco::table_generator<person>(persons)
            .add_column
            (
               "First name",
               [](const person& p) -> std::string { return p.first; }
            )
            .add_column
            (
               "Last name",
               [](const person& p) -> std::string { return p.last; }
            )
            .to_table()
            ;
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 5);
        const auto first = std::vector<std::string> { "First name", "Jerry", "Elaine", "Cosmo", "George"};
        const auto last = std::vector<std::string> { "Last name", "Seinfeld", "Benes", "Kramer", "Costanza"};
        CHECK(euco::calc_column_as_vector(table, 0) == first);
        CHECK(euco::calc_column_as_vector(table, 1) == last);
        // todo(Gustav): add print tests
        // PrintTableSimple(std::cout, table);
    }

    SECTION("sort on last name")
    {
        const auto table = euco::table_generator<person>(persons)
            .add_column
            (
                "First name",
                [](const person& p) -> std::string { return p.first; }
            )
            .add_column
            (
                "Last name",
                [](const person& p) -> std::string { return p.last; }
            )
            .sort(&person::last)
            .to_table()
            ;
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 5);
        const auto first = std::vector<std::string> { "First name", "Elaine", "George", "Cosmo", "Jerry" };
        const auto last = std::vector<std::string> { "Last name", "Benes", "Costanza", "Kramer", "Seinfeld" };
        CHECK(euco::calc_column_as_vector(table, 0) == first);
        CHECK(euco::calc_column_as_vector(table, 1) == last);
        // PrintTableGrid(std::cout, table);
    }
}

TEST_CASE("stringtable-from_csv_default", "[stringtable]")
{
    const auto first_column = std::vector<std::string> {"a", "1"};
    const auto first_column_string = std::vector<std::string> {"a b", "1"};
    const auto second_column = std::vector<std::string> {"b", "2"};
    const auto second_column_space = std::vector<std::string> {" b ", "2"};
    const auto ab12 = std::vector<std::string> {"ab", "12"};

    SECTION("not ending with newline")
    {
        const auto table = euco::table_from_csv("a,b\n1,2");
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == first_column);
        CHECK(euco::calc_column_as_vector(table, 1) == second_column);
    }

    SECTION("single column")
    {
        const auto table = euco::table_from_csv("ab\n12");
        REQUIRE(table.get_width() == 1);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == ab12);
    }

    SECTION("ending with newlines")
    {
        const auto table = euco::table_from_csv("a,b\n\n\n1,2\n\n");
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == first_column);
        CHECK(euco::calc_column_as_vector(table, 1) == second_column);
    }

    SECTION("with spaces (trim)")
    {
        euco::csv_parser_options options;
        options.trim = euco::csv_trim::trim;

        const auto table = euco::table_from_csv("  a  ,  b  \n  1    ,  2  ", options);
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == first_column);
        CHECK(euco::calc_column_as_vector(table, 1) == second_column);
    }

    SECTION("with spaces (keep)")
    {
        const auto table = euco::table_from_csv("a, b \n1,2");
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == first_column);
        CHECK(euco::calc_column_as_vector(table, 1) == second_column_space);
    }

    SECTION("with spaces (string)")
    {
        euco::csv_parser_options options;
        options.trim = euco::csv_trim::trim;

        const auto table = euco::table_from_csv("a,   \" b \"   \n1,2", options);
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == first_column);
        CHECK(euco::calc_column_as_vector(table, 1) == second_column_space);
    }

    SECTION("strings")
    {
        const auto table = euco::table_from_csv("\"a b\",b\n1,2");
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == first_column_string);
        CHECK(euco::calc_column_as_vector(table, 1) == second_column);
    }

    SECTION("not ending strings single col")
    {
        const auto table = euco::table_from_csv("\"a b");
        const auto error_column = std::vector<std::string> {"a b"};
        REQUIRE(table.get_width() == 1);
        REQUIRE(table.get_height() == 1);
        CHECK(euco::calc_column_as_vector(table, 0) == error_column);
    }

    SECTION("not ending strings 2 cols")
    {
        const auto table = euco::table_from_csv("err,\"a b");
        const auto error_column_1 = std::vector<std::string> {"err"};
        const auto error_column_2 = std::vector<std::string> {"a b"};
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 1);
        CHECK(euco::calc_column_as_vector(table, 0) == error_column_1);
        CHECK(euco::calc_column_as_vector(table, 1) == error_column_2);
    }

    SECTION("string with quote")
    {
        const auto table = euco::table_from_csv("\"a \"\" b\"");
        const auto col = std::vector<std::string> {"a \" b"};
        REQUIRE(table.get_width() == 1);
        REQUIRE(table.get_height() == 1);
        CHECK(euco::calc_column_as_vector(table, 0) == col);
    }
}

TEST_CASE("stringtable-from_csv_not_default", "[stringtable]")
{
    const auto first_column = std::vector<std::string> {"a", "1"};
    const auto first_column_string = std::vector<std::string> {"a b", "1"};
    const auto second_column = std::vector<std::string> {"b", "2"};

    euco::csv_parser_options options;
    options.delim = 'c';
    options.str = 's';

    SECTION("not ending with newline")
    {
        const auto table = euco::table_from_csv("acb\n1c2", options);
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == first_column);
        CHECK(euco::calc_column_as_vector(table, 1) == second_column);
    }

    SECTION("ending with newlines")
    {
        const auto table = euco::table_from_csv("acb\n\n\n1c2\n\n", options);
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == first_column);
        CHECK(euco::calc_column_as_vector(table, 1) == second_column);
    }

    SECTION("strings")
    {
        const auto table = euco::table_from_csv("sa bscb\n1c2", options);
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 2);
        CHECK(euco::calc_column_as_vector(table, 0) == first_column_string);
        CHECK(euco::calc_column_as_vector(table, 1) == second_column);
    }

    SECTION("not ending strings single col")
    {
        const auto table = euco::table_from_csv("sa b", options);
        const auto errorcol = std::vector<std::string> {"a b"};
        REQUIRE(table.get_width() == 1);
        REQUIRE(table.get_height() == 1);
        CHECK(euco::calc_column_as_vector(table, 0) == errorcol);
    }

    SECTION("not ending strings 2 cols")
    {
        const auto table = euco::table_from_csv("errcsa b", options);
        const auto error_column_1 = std::vector<std::string> {"err"};
        const auto error_column_2 = std::vector<std::string> {"a b"};
        REQUIRE(table.get_width() == 2);
        REQUIRE(table.get_height() == 1);
        CHECK(euco::calc_column_as_vector(table, 0) == error_column_1);
        CHECK(euco::calc_column_as_vector(table, 1) == error_column_2);
    }

    SECTION("string with quote")
    {
        const auto table = euco::table_from_csv("sa ss bs", options);
        const auto col = std::vector<std::string> {"a s b"};
        REQUIRE(table.get_width() == 1);
        REQUIRE(table.get_height() == 1);
        CHECK(euco::calc_column_as_vector(table, 0) == col);
    }
}
