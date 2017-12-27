#include "core/chatbot.h"

#include "catch.hpp"

using namespace chatbot;

TEST_CASE("chatbot-matches-input", "[chatbot]")
{
  const std::string full_sentence = "cat dog fish";
  const auto        sentence      = CleanInput(full_sentence);

  const std::string start_animal  = "cat";
  const std::string middle_animal = "dog";
  const std::string end_animal    = "fish";
  const std::string non_animal    = "human";

  SECTION("match input")
  {
    const auto where = Input::IN_MIDDLE;
    CHECK(MatchesInputVector(sentence, Input{start_animal, where}));
    CHECK(MatchesInputVector(sentence, Input{middle_animal, where}));
    CHECK(MatchesInputVector(sentence, Input{end_animal, where}));
    CHECK(MatchesInputVector(sentence, Input{full_sentence, where}));
    CHECK_FALSE(MatchesInputVector(sentence, Input{non_animal, where}));
  }

  SECTION("match start")
  {
    const auto where = Input::AT_START;
    CHECK(MatchesInputVector(sentence, Input{start_animal, where}));
    CHECK_FALSE(MatchesInputVector(sentence, Input{middle_animal, where}));
    CHECK_FALSE(MatchesInputVector(sentence, Input{end_animal, where}));
    CHECK(MatchesInputVector(sentence, Input{full_sentence, where}));
    CHECK_FALSE(MatchesInputVector(sentence, Input{non_animal, where}));
  }

  SECTION("match end")
  {
    const auto where = Input::AT_END;
    CHECK_FALSE(MatchesInputVector(sentence, Input{start_animal, where}));
    CHECK_FALSE(MatchesInputVector(sentence, Input{middle_animal, where}));
    CHECK(MatchesInputVector(sentence, Input{end_animal, where}));
    CHECK(MatchesInputVector(sentence, Input{full_sentence, where}));
    CHECK_FALSE(MatchesInputVector(sentence, Input{non_animal, where}));
  }

  SECTION("alone")
  {
    const auto where = Input::ALONE;
    CHECK_FALSE(MatchesInputVector(sentence, Input{start_animal, where}));
    CHECK_FALSE(MatchesInputVector(sentence, Input{middle_animal, where}));
    CHECK_FALSE(MatchesInputVector(sentence, Input{end_animal, where}));
    CHECK(MatchesInputVector(sentence, Input{full_sentence, where}));
    CHECK_FALSE(MatchesInputVector(sentence, Input{non_animal, where}));
  }
}
