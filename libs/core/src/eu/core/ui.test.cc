
#include "eu/core/ui.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include "eu/tests/false_string.h"

#include <string>

using namespace eu;
using namespace eu::tests;
using namespace eu::core;

static FalseString vec2_match(const v2& left, const v2& right)
{
	using W = Catch::Matchers::WithinAbsMatcher;
	constexpr double m = 0.0001;
	const auto same = W(left.x, m).match(right.x) && W(left.y, m).match(right.y);
	return same ? FalseString::True()
				: FalseString::False(fmt::format("{} != {}", string_from(left), string_from(right)));
}

TEST_CASE("ui-region-works", "[ui]")
{
	// todo(Gustav): expand tests
	CHECK(vec2_match(calculate_region({0, 0}, {0, 0}, {0, 0}, {10, 10}, 10), {0, 0}));
}
