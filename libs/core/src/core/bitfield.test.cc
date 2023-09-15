#include "core/bitfield.h"

#include "catch.hpp"

using namespace eu::core;

namespace
{
    enum class Bits
    {
        bit1 = 1 << 1,
        bit2 = 1 << 2,
        bit3 = 1 << 3
    };
}

TEST_CASE("bitfield-basic", "[bitfield]")
{
    Bitfield<Bits> b;

    SECTION("Empty")
    {
        CHECK_FALSE(b.has_any_values());
        CHECK_FALSE(b.get(Bits::bit1));
        CHECK_FALSE(b.get(Bits::bit2));
        CHECK_FALSE(b.get(Bits::bit3));
    }

    SECTION("Set false")
    {
        b.set(Bits::bit1, false);
        b.set(Bits::bit2, false);
        b.set(Bits::bit3, false);

        CHECK_FALSE(b.has_any_values());

        CHECK_FALSE(b.get(Bits::bit1));
        CHECK_FALSE(b.get(Bits::bit2));
        CHECK_FALSE(b.get(Bits::bit3));
    }

    SECTION("Some")
    {
        b.set(Bits::bit1, true);

        CHECK(b.has_any_values());
        CHECK(b.get(Bits::bit1));
        CHECK_FALSE(b.get(Bits::bit2));
        CHECK_FALSE(b.get(Bits::bit3));
    }

    SECTION("Toggle")
    {
        b.toggle(Bits::bit3);
        CHECK_FALSE(b.get(Bits::bit1));
        CHECK_FALSE(b.get(Bits::bit2));
        CHECK(b.get(Bits::bit3));
    }

    SECTION("All")
    {
        b.set(Bits::bit1, true);
        b.set(Bits::bit2, true);
        b.set(Bits::bit3, true);
        CHECK(b.get(Bits::bit1));
        CHECK(b.get(Bits::bit2));
        CHECK(b.get(Bits::bit3));

        b.clear();
        CHECK_FALSE(b.get(Bits::bit1));
        CHECK_FALSE(b.get(Bits::bit2));
        CHECK_FALSE(b.get(Bits::bit3));
    }
}
