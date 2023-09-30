#include "core/findstring.h"

#include "catch.hpp"

namespace core = eu::core;

TEST_CASE("findstring 1", "[findstring]")
{
    CHECK(core::find("abc", "a"));
    CHECK(core::find("abc", "b"));
    CHECK(core::find("abc", "c"));

    CHECK_FALSE(core::find("abc", "d"));
}

TEST_CASE("findstring 2", "[findstring]")
{
    const auto abc = std::vector<std::string> {"a", "b", "c"};

    // check for a, b or c is in any of the lists entries
    CHECK(core::find(abc, "a"));
    CHECK(core::find(abc, "b"));
    CHECK(core::find(abc, "c"));
    CHECK_FALSE(core::find(abc, "d"));

    // must contain a, b AND c
    CHECK(core::find("aabbcc", abc));
    CHECK(core::find("aabbcc", abc));
    CHECK(core::find("bcabca", abc));
    CHECK(core::find("cab", abc));
    // doesnt contain a, b AND c
    CHECK_FALSE(core::find("dcb", abc));
}

TEST_CASE("findstring 3", "[findstring]")
{
    const auto animals = std::vector<std::string> {"cat and dog with hat",
                                                   "kitty cat and doggo",
                                                   "dog and cat",
                                                   "horse, cat and dog"};

    const auto pets = std::vector<std::string> {"cat", "dog"};
    const auto garfield = std::vector<std::string> {"cat", "dog", "human"};

    CHECK(core::find(animals, "dog"));
    CHECK(core::find(animals, "cat"));
    CHECK_FALSE(core::find(animals, "human"));

    CHECK(core::find(animals, pets));
    CHECK_FALSE(core::find(animals, garfield));
}
