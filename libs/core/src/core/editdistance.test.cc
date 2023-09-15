#include "core/editdistance.h"

#include "catch.hpp"

namespace euco = eu::core;

TEST_CASE("editdistance", "[editdistance]")
{
    const std::string a = "kitten";
    const std::string b = "sitting";
    const unsigned long distance = 3;

    CHECK(euco::calc_edit_distance_fast(a, b) == distance);
    CHECK(euco::calc_edit_distance_fast(b, a) == distance);

    CHECK(euco::calc_edit_distance(a, b) == distance);
    CHECK(euco::calc_edit_distance(b, a) == distance);
}

TEST_CASE("editdistance2", "[editdistance]")
{
    const std::string a = "abc";
    const std::string b = "abcde";
    const unsigned long distance = 2;

    CHECK(euco::calc_edit_distance_fast(a, b) == distance);
    CHECK(euco::calc_edit_distance_fast(b, a) == distance);

    CHECK(euco::calc_edit_distance(a, b) == distance);
    CHECK(euco::calc_edit_distance(b, a) == distance);
}

TEST_CASE("editdistance-codeproject", "[editdistance]")
{
    const std::string a = "GUMBO";
    const std::string b = "GAMBOL";
    const unsigned long distance = 2;

    CHECK(euco::calc_edit_distance_fast(a, b) == distance);
    CHECK(euco::calc_edit_distance_fast(b, a) == distance);

    CHECK(euco::calc_edit_distance(a, b) == distance);
    CHECK(euco::calc_edit_distance(b, a) == distance);
}
