#include "core/findstring.h"

#include "catch.hpp"

TEST_CASE("findstring 1", "[findstring]")
{
  CHECK(Find("abc", "a"));
  CHECK(Find("abc", "b"));
  CHECK(Find("abc", "c"));

  CHECK_FALSE(Find("abc", "d"));
}

TEST_CASE("findstring 2", "[findstring]")
{
  const auto abc    = std::vector<std::string>{"a", "b", "c"};

  // check for a, b or c is in any of the lists entries
  CHECK(Find(abc, "a"));
  CHECK(Find(abc, "b"));
  CHECK(Find(abc, "c"));
  CHECK_FALSE(Find(abc, "d"));

  // must contain a, b AND c
  CHECK(Find("aabbcc", abc));
  CHECK(Find("aabbcc", abc));
  CHECK(Find("bcabca", abc));
  CHECK(Find("cab", abc));
  // doesnt contain a, b AND c
  CHECK_FALSE(Find("dcb", abc));
}

TEST_CASE("findstring 3", "[findstring]")
{
  const auto animals = std::vector<std::string>{"cat and dog with hat",
                                                "kitty cat and doggo",
                                                "dog and cat",
                                                "horse, cat and dog"};

  const auto pets     = std::vector<std::string>{"cat", "dog"};
  const auto garfield = std::vector<std::string>{"cat", "dog", "human"};

  CHECK(Find(animals, "dog"));
  CHECK(Find(animals, "cat"));
  CHECK_FALSE(Find(animals, "human"));

  CHECK(Find(animals, pets));
  CHECK_FALSE(Find(animals, garfield));
}
