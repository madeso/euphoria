#include "core/nlp_sentence.h"

#include "tests/approx_equal.h"
#include "tests/stringeq.h"

#include "catch.hpp"


using namespace euphoria::tests;
using namespace euphoria::core;

namespace
{
    std::pair<bool, std::vector<TextSentence>>
    parse_to_sentence(const std::string& str)
    {
        std::vector<TextSentence> list;

        auto ss = std::istringstream{str};

        const auto result = parse_sentences(ss, [&](const TextSentence& s){list.emplace_back(s);});

        return {result, list};
    }
}


TEST_CASE("nlp-sentence", "[nlp]")
{
    SECTION("simple parse")
    {
        const auto [result, list] = parse_to_sentence("Hello world");
        REQUIRE(result);
        REQUIRE(list.size() == 1);
        REQUIRE(string_is_equal(list[0], {"Hello", "world"}));
    }

    SECTION("advanced parse")
    {
        const auto [result, list] = parse_to_sentence("Dog is good? Lists are super-long vectors.");
        REQUIRE(result);
        REQUIRE(list.size() == 2);
        REQUIRE(string_is_equal(list[0], {"Dog", "is", "good", "?"}));
        REQUIRE(string_is_equal(list[1], {"Lists", "are", "super-long", "vectors", "."}));
    }

    SECTION("to string")
    {
        REQUIRE(string_is_equal("Hello world, I am dog.",
                                sentence_to_string({"Hello", "world", ",", "I", "am", "dog", "."})));
    }
}

