#include "core/bitfield.h"

#include "catch.hpp"

using namespace euphoria::core;

namespace
{
    enum class B
    {
        A = 1 << 1,
        B = 1 << 2,
        C = 1 << 3
    };
}

TEST_CASE("bitfield-basic", "[bitfield]")
{
    bitfield<B> b;

    SECTION("Empty")
    {
        CHECK_FALSE(b.has_any_values());
        CHECK_FALSE(b.get(B::A));
        CHECK_FALSE(b.get(B::B));
        CHECK_FALSE(b.get(B::C));
    }

    SECTION("Set false")
    {
        b.set(B::A, false);
        b.set(B::B, false);
        b.set(B::C, false);

        CHECK_FALSE(b.has_any_values());

        CHECK_FALSE(b.get(B::A));
        CHECK_FALSE(b.get(B::B));
        CHECK_FALSE(b.get(B::C));
    }

    SECTION("Some")
    {
        b.set(B::A, true);

        CHECK(b.has_any_values());
        CHECK(b.get(B::A));
        CHECK_FALSE(b.get(B::B));
        CHECK_FALSE(b.get(B::C));
    }

    SECTION("Toggle")
    {
        b.toggle(B::C);
        CHECK_FALSE(b.get(B::A));
        CHECK_FALSE(b.get(B::B));
        CHECK(b.get(B::C));
    }

    SECTION("All")
    {
        b.set(B::A, true);
        b.set(B::B, true);
        b.set(B::C, true);
        CHECK(b.get(B::A));
        CHECK(b.get(B::B));
        CHECK(b.get(B::C));

        b.clear();
        CHECK_FALSE(b.get(B::A));
        CHECK_FALSE(b.get(B::B));
        CHECK_FALSE(b.get(B::C));
    }
}
