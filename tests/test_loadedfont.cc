#include "core/loadedfont.h"

#include "approx_equal.h"
#include "stringeq.h"

#include "catch.hpp"


using namespace euphoria::tests;
using namespace euphoria::core;


TEST_CASE("fonts-", "[fonts]")
{
    auto font = LoadedFont{};

    SECTION("private use")
    {
        const auto acp = font.NewPrivateUse("a");
        const auto it = font.private_use_aliases.find("a");
        const auto found = it != font.private_use_aliases.end();
        REQUIRE(found);
        CHECK(it->second == acp);
    }

    SECTION("combine private use")
    {
        constexpr int a_advance = 10;
        constexpr int b_advance = 20;

        auto a = LoadedGlyph{};
        a.valid = true;
        a.advance = a_advance;

        auto b = LoadedGlyph{};
        b.valid = true;
        b.advance = b_advance;

        auto load = [](const std::string& name, const LoadedGlyph& g) -> LoadedFont
        {
            auto f = LoadedFont{};
            const auto cp = f.NewPrivateUse(name);
            f.codepoint_to_glyph[cp] = g;
            return f;
        };
        font.CombineWith(load("a", a));
        font.CombineWith(load("b", b));

        {
            const auto codepoint_iterator = font.private_use_aliases.find("a");
            const auto found_codepoint =
                codepoint_iterator != font.private_use_aliases.end();
            REQUIRE(found_codepoint);
            const auto glyph_iterator = font.codepoint_to_glyph.find
            (
                codepoint_iterator->second
            );
            const auto found_glyph =
                glyph_iterator != font.codepoint_to_glyph.end();
            REQUIRE(found_glyph);
            CHECK(glyph_iterator->second.advance == a_advance);
        }

        {
            const auto codepoint_iterator = font.private_use_aliases.find("b");
            const auto found_codepoint =
                codepoint_iterator != font.private_use_aliases.end();
            REQUIRE(found_codepoint);
            const auto glyph_iterator = font.codepoint_to_glyph.find
            (
                codepoint_iterator->second
            );
            const auto found_glyph =
                glyph_iterator != font.codepoint_to_glyph.end();
            REQUIRE(found_glyph);
            CHECK(glyph_iterator->second.advance == b_advance);
        }
    }
}

