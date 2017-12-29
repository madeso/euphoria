#include "core/chatbot.h"

#include "catch.hpp"

using namespace chatbot;

TEST_CASE("chatbot-matches-input", "[chatbot]")
{
  const std::string full_sentence = "cat dog fish";
  const auto        sentence      = CleanInput(full_sentence);

  const auto without_start_animal  = CleanInput("dog fish");
  const auto without_middle_animal = CleanInput("cat fish");
  const auto without_end_animal    = CleanInput("cat dog");
  const auto without_any_animals   = CleanInput("");

  const std::string start_animal  = "cat";
  const std::string middle_animal = "dog";
  const std::string end_animal    = "fish";
  const std::string non_animal    = "human";

  SECTION("match middle")
  {
    const auto where = Input::IN_MIDDLE;
    CHECK(IndexOfMatchedInput(sentence, Input{start_animal, where}) == 0);
    CHECK(IndexOfMatchedInput(sentence, Input{middle_animal, where}) == 1);
    CHECK(IndexOfMatchedInput(sentence, Input{end_animal, where}) == 2);
    CHECK(IndexOfMatchedInput(sentence, Input{full_sentence, where}) == 0);
    CHECK(IndexOfMatchedInput(sentence, Input{non_animal, where}) == -1);
  }

  SECTION("match start")
  {
    const auto where = Input::AT_START;
    CHECK(IndexOfMatchedInput(sentence, Input{start_animal, where}) == 0);
    CHECK(IndexOfMatchedInput(sentence, Input{middle_animal, where}) == -1);
    CHECK(IndexOfMatchedInput(sentence, Input{end_animal, where}) == -1);
    CHECK(IndexOfMatchedInput(sentence, Input{full_sentence, where}) == 0);
    CHECK(IndexOfMatchedInput(sentence, Input{non_animal, where}) == -1);
  }

  SECTION("match end")
  {
    const auto where = Input::AT_END;
    CHECK(IndexOfMatchedInput(sentence, Input{start_animal, where}) == -1);
    CHECK(IndexOfMatchedInput(sentence, Input{middle_animal, where}) == -1);
    CHECK(IndexOfMatchedInput(sentence, Input{end_animal, where}) == 2);
    CHECK(IndexOfMatchedInput(sentence, Input{full_sentence, where}) == 0);
    CHECK(IndexOfMatchedInput(sentence, Input{non_animal, where}) == -1);
  }

  SECTION("alone")
  {
    const auto where = Input::ALONE;
    CHECK(IndexOfMatchedInput(sentence, Input{start_animal, where}) == -1);
    CHECK(IndexOfMatchedInput(sentence, Input{middle_animal, where}) == -1);
    CHECK(IndexOfMatchedInput(sentence, Input{end_animal, where}) == -1);
    CHECK(IndexOfMatchedInput(sentence, Input{full_sentence, where}) == 0);
    CHECK(IndexOfMatchedInput(sentence, Input{non_animal, where}) == -1);
  }

  SECTION("remove from middle")
  {
    const auto where = Input::IN_MIDDLE;
    CHECK(
        RemoveFrom(sentence, Input{start_animal, where}) ==
        without_start_animal);
    CHECK(
        RemoveFrom(sentence, Input{middle_animal, where}) ==
        without_middle_animal);
    CHECK(RemoveFrom(sentence, Input{end_animal, where}) == without_end_animal);
    CHECK(
        RemoveFrom(sentence, Input{full_sentence, where}) ==
        without_any_animals);
    CHECK(RemoveFrom(sentence, Input{non_animal, where}) == sentence);
  }
}
