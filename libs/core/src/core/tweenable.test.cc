#include "core/tweenable.h"

#include "tests/approx_equal.h"

#include "catch.hpp"


namespace euco = eu::core;

TEST_CASE("tweenable-do_nothing", "[tweenable]")
{
    euco::Tweenable<float> f {5.0f};
    f.update(0.1f);

    REQUIRE(f.value == Approx(5.0f));
}

TEST_CASE("tweenable-basic", "[tweenable]")
{
    euco::Tweenable<float> f {0.0f};
    f.set(1.0f, euco::easing::Function::linear, 1.0f);
    f.update(0.1f);

    REQUIRE(f.value == Approx(0.1f));
}

TEST_CASE("tweenable-basic_negative", "[tweenable]")
{
    euco::Tweenable<float> f {1.0f};
    f.set(0.0f, euco::easing::Function::linear, 1.0f);
    f.update(0.1f);

    REQUIRE(f.value == Approx(0.9f));
}

TEST_CASE("tweenable-basic_with_clear", "[tweenable]")
{
    euco::Tweenable<float> f {0.0f};
    f.clear().set(1.0f, euco::easing::Function::linear, 1.0f);
    f.update(0.1f);

    REQUIRE(f.value == Approx(0.1f));
}

TEST_CASE("tweenable-huge_step", "[tweenable]")
{
    euco::Tweenable<float> f {0.0f};
    f.set(1.0f, euco::easing::Function::linear, 1.0f);
    f.update(5.0f);

    REQUIRE_FALSE(f.is_active());
    REQUIRE(f.value == Approx(1.0f));
}

TEST_CASE("tweenable-clear_after_a_time", "[tweenable]")
{
    euco::Tweenable<float> f {0.0f};
    f.set(1.0f, euco::easing::Function::linear, 1.0f);
    f.update(0.5f);
    REQUIRE(f.is_active());
    f.clear();

    REQUIRE_FALSE(f.is_active());
    REQUIRE(f.value == Approx(0.5f));
}

TEST_CASE("tweenable-change_after_some_time", "[tweenable]")
{
    euco::Tweenable<float> f {0.0f};
    f.set(2.0f, euco::easing::Function::linear, 1.0f); // go to 2 this time
    f.update(0.5f); // go half
    REQUIRE(f.value == Approx(1.0f));
    f.clear().set(0.0f, euco::easing::Function::linear, 1.0f); // go back to 0
    REQUIRE(f.value == Approx(1.0f));
    REQUIRE(f.from == Approx(1.0f));
    f.update(0.5f);
    REQUIRE(f.is_active());
    REQUIRE(f.value == Approx(0.5f));
}
