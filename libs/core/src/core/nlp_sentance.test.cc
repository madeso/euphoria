#include "core/nlp_sentance.h"

#include "tests/approx_equal.h"
#include "tests/stringeq.h"

#include "catch.hpp"


using namespace euphoria::tests;
using namespace euphoria::core;

namespace
{
    std::pair<bool, std::vector<Sentance>>
    Parse(const std::string& str)
    {
        std::vector<Sentance> list;

        auto ss = std::istringstream{str};

        const auto result = ParseSentances(ss, [&](const Sentance& s){list.emplace_back(s);});

        return {result, list};
    }
}


TEST_CASE("nlp-sentance", "[nlp]")
{
    SECTION("simple parse")
    {
        const auto [result, list] = Parse("Hello world");
        REQUIRE(result);
        REQUIRE(list.size() == 1);
        REQUIRE(StringEq(list[0], {"Hello", "world"}));
    }

    SECTION("advanced parse")
    {
        const auto [result, list] = Parse("Dog is good? Lists are super-long vectors.");
        REQUIRE(result);
        REQUIRE(list.size() == 2);
        REQUIRE(StringEq(list[0], {"Dog", "is", "good", "?"}));
        REQUIRE(StringEq(list[1], {"Lists", "are", "super-long", "vectors", "."}));
    }

    SECTION("to string")
    {
        REQUIRE(StringEq("Hello world, I am dog.", SentanceToString({"Hello", "world", ",", "I", "am", "dog", "."})));
    }
}

