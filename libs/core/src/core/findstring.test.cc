#include "core/findstring.h"

#include "catch.hpp"

namespace euco = eu::core;

TEST_CASE("findstring 1", "[findstring]")
{
    CHECK(euco::find("abc", "a"));
    CHECK(euco::find("abc", "b"));
    CHECK(euco::find("abc", "c"));

    CHECK_FALSE(euco::find("abc", "d"));
}

TEST_CASE("findstring 2", "[findstring]")
{
    const auto abc = std::vector<std::string> {"a", "b", "c"};

    // check for a, b or c is in any of the lists entries
    CHECK(euco::find(abc, "a"));
    CHECK(euco::find(abc, "b"));
    CHECK(euco::find(abc, "c"));
    CHECK_FALSE(euco::find(abc, "d"));

    // must contain a, b AND c
    CHECK(euco::find("aabbcc", abc));
    CHECK(euco::find("aabbcc", abc));
    CHECK(euco::find("bcabca", abc));
    CHECK(euco::find("cab", abc));
    // doesnt contain a, b AND c
    CHECK_FALSE(euco::find("dcb", abc));
}

TEST_CASE("findstring 3", "[findstring]")
{
    const auto animals = std::vector<std::string> {"cat and dog with hat",
                                                   "kitty cat and doggo",
                                                   "dog and cat",
                                                   "horse, cat and dog"};

    const auto pets = std::vector<std::string> {"cat", "dog"};
    const auto garfield = std::vector<std::string> {"cat", "dog", "human"};

    CHECK(euco::find(animals, "dog"));
    CHECK(euco::find(animals, "cat"));
    CHECK_FALSE(euco::find(animals, "human"));

    CHECK(euco::find(animals, pets));
    CHECK_FALSE(euco::find(animals, garfield));
}
