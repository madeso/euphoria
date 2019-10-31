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
    Bitfield<B> b;

    SECTION("Empty")
    {
        CHECK_FALSE(b.HasAnyValues());
        CHECK_FALSE(b.Get(B::A));
        CHECK_FALSE(b.Get(B::B));
        CHECK_FALSE(b.Get(B::C));
    }

    SECTION("Set false")
    {
        b.Set(B::A, false);
        b.Set(B::B, false);
        b.Set(B::C, false);

        CHECK_FALSE(b.HasAnyValues());

        CHECK_FALSE(b.Get(B::A));
        CHECK_FALSE(b.Get(B::B));
        CHECK_FALSE(b.Get(B::C));
    }

    SECTION("Some")
    {
        b.Set(B::A, true);

        CHECK(b.HasAnyValues());
        CHECK(b.Get(B::A));
        CHECK_FALSE(b.Get(B::B));
        CHECK_FALSE(b.Get(B::C));
    }

    SECTION("Toggle")
    {
        b.Toggle(B::C);
        CHECK_FALSE(b.Get(B::A));
        CHECK_FALSE(b.Get(B::B));
        CHECK(b.Get(B::C));
    }

    SECTION("All")
    {
        b.Set(B::A, true);
        b.Set(B::B, true);
        b.Set(B::C, true);
        CHECK(b.Get(B::A));
        CHECK(b.Get(B::B));
        CHECK(b.Get(B::C));

        b.Clear();
        CHECK_FALSE(b.Get(B::A));
        CHECK_FALSE(b.Get(B::B));
        CHECK_FALSE(b.Get(B::C));
    }
}
