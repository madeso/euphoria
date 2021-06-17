#include "core/bitfield.h"

#include "catch.hpp"

using namespace euphoria::core;

namespace
{
    enum class bits
    {
        bit1 = 1 << 1,
        bit2 = 1 << 2,
        bit3 = 1 << 3
    };
}

TEST_CASE("bitfield-basic", "[bitfield]")
{
    bitfield<bits> b;

    SECTION("Empty")
    {
        CHECK_FALSE(b.has_any_values());
        CHECK_FALSE(b.get(bits::bit1));
        CHECK_FALSE(b.get(bits::bit2));
        CHECK_FALSE(b.get(bits::bit3));
    }

    SECTION("Set false")
    {
        b.set(bits::bit1, false);
        b.set(bits::bit2, false);
        b.set(bits::bit3, false);

        CHECK_FALSE(b.has_any_values());

        CHECK_FALSE(b.get(bits::bit1));
        CHECK_FALSE(b.get(bits::bit2));
        CHECK_FALSE(b.get(bits::bit3));
    }

    SECTION("Some")
    {
        b.set(bits::bit1, true);

        CHECK(b.has_any_values());
        CHECK(b.get(bits::bit1));
        CHECK_FALSE(b.get(bits::bit2));
        CHECK_FALSE(b.get(bits::bit3));
    }

    SECTION("Toggle")
    {
        b.toggle(bits::bit3);
        CHECK_FALSE(b.get(bits::bit1));
        CHECK_FALSE(b.get(bits::bit2));
        CHECK(b.get(bits::bit3));
    }

    SECTION("All")
    {
        b.set(bits::bit1, true);
        b.set(bits::bit2, true);
        b.set(bits::bit3, true);
        CHECK(b.get(bits::bit1));
        CHECK(b.get(bits::bit2));
        CHECK(b.get(bits::bit3));

        b.clear();
        CHECK_FALSE(b.get(bits::bit1));
        CHECK_FALSE(b.get(bits::bit2));
        CHECK_FALSE(b.get(bits::bit3));
    }
}
