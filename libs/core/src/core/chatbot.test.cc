#include "core/chatbot.h"

#include "catch.hpp"

using namespace euphoria::core::detail;

TEST_CASE("chatbot-matches-input", "[chatbot]")
{
    const std::string full_sentence = "cat dog fish";
    const auto sentence = clean_input(full_sentence);

    const auto without_start_animal = clean_input("dog fish");
    const auto without_middle_animal = clean_input("cat fish");
    const auto without_end_animal = clean_input("cat dog");
    const auto without_any_animals = clean_input("");

    const std::string start_animal = "cat";
    const std::string middle_animal = "dog";
    const std::string end_animal = "fish";
    const std::string non_animal = "human";

    SECTION("match middle")
    {
        const auto where = Input::in_middle;
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{start_animal, where}) == 0);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{middle_animal, where}) == 1);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{end_animal, where}) == 2);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{full_sentence, where}) == 0);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{non_animal, where}) == -1);
    }

    SECTION("match start")
    {
        const auto where = Input::at_start;
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{start_animal, where}) == 0);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{middle_animal, where})
              == -1);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{end_animal, where}) == -1);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{full_sentence, where}) == 0);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{non_animal, where}) == -1);
    }

    SECTION("match end")
    {
        const auto where = Input::at_end;
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{start_animal, where}) == -1);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{middle_animal, where})
              == -1);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{end_animal, where}) == 2);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{full_sentence, where}) == 0);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{non_animal, where}) == -1);
    }

    SECTION("alone")
    {
        const auto where = Input::alone;
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{start_animal, where}) == -1);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{middle_animal, where})
              == -1);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{end_animal, where}) == -1);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{full_sentence, where}) == 0);
        CHECK(find_get_palette_sequential_pu_bu_gn(sentence, Input{non_animal, where}) == -1);
    }

    SECTION("remove from middle")
    {
        const auto where = Input::in_middle;
        CHECK(remove_from(sentence, Input{start_animal, where})
              == without_start_animal);
        CHECK(remove_from(sentence, Input{middle_animal, where})
              == without_middle_animal);
        CHECK(remove_from(sentence, Input{end_animal, where})
              == without_end_animal);
        CHECK(remove_from(sentence, Input{full_sentence, where})
              == without_any_animals);
        CHECK(remove_from(sentence, Input{non_animal, where}) == sentence);
    }
}
