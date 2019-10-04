#include "core/findstring.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("findstring 1", "[findstring]")
{
  CHECK(euco::Find("abc", "a"));
  CHECK(euco::Find("abc", "b"));
  CHECK(euco::Find("abc", "c"));

  CHECK_FALSE(euco::Find("abc", "d"));
}

TEST_CASE("findstring 2", "[findstring]")
{
  const auto abc    = std::vector<std::string>{"a", "b", "c"};

  // check for a, b or c is in any of the lists entries
  CHECK(euco::Find(abc, "a"));
  CHECK(euco::Find(abc, "b"));
  CHECK(euco::Find(abc, "c"));
  CHECK_FALSE(euco::Find(abc, "d"));

  // must contain a, b AND c
  CHECK(euco::Find("aabbcc", abc));
  CHECK(euco::Find("aabbcc", abc));
  CHECK(euco::Find("bcabca", abc));
  CHECK(euco::Find("cab", abc));
  // doesnt contain a, b AND c
  CHECK_FALSE(euco::Find("dcb", abc));
}

TEST_CASE("findstring 3", "[findstring]")
{
  const auto animals = std::vector<std::string>{"cat and dog with hat",
                                                "kitty cat and doggo",
                                                "dog and cat",
                                                "horse, cat and dog"};

  const auto pets     = std::vector<std::string>{"cat", "dog"};
  const auto garfield = std::vector<std::string>{"cat", "dog", "human"};

  CHECK(euco::Find(animals, "dog"));
  CHECK(euco::Find(animals, "cat"));
  CHECK_FALSE(euco::Find(animals, "human"));

  CHECK(euco::Find(animals, pets));
  CHECK_FALSE(euco::Find(animals, garfield));
}
