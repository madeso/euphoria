#include "core/editdistance.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("editdistance", "[editdistance]")
{
    const std::string   a        = "kitten";
    const std::string   b        = "sitting";
    const unsigned long distance = 3;

    CHECK(euco::fast_edit_distance(a, b) == distance);
    CHECK(euco::fast_edit_distance(b, a) == distance);

    CHECK(euco::edit_distance(a, b) == distance);
    CHECK(euco::edit_distance(b, a) == distance);
}

TEST_CASE("editdistance2", "[editdistance]")
{
    const std::string   a        = "abc";
    const std::string   b        = "abcde";
    const unsigned long distance = 2;

    CHECK(euco::fast_edit_distance(a, b) == distance);
    CHECK(euco::fast_edit_distance(b, a) == distance);

    CHECK(euco::edit_distance(a, b) == distance);
    CHECK(euco::edit_distance(b, a) == distance);
}

TEST_CASE("editdistance-codeproject", "[editdistance]")
{
    const std::string   a        = "GUMBO";
    const std::string   b        = "GAMBOL";
    const unsigned long distance = 2;

    CHECK(euco::fast_edit_distance(a, b) == distance);
    CHECK(euco::fast_edit_distance(b, a) == distance);

    CHECK(euco::edit_distance(a, b) == distance);
    CHECK(euco::edit_distance(b, a) == distance);
}
