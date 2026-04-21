
#include "eu/core/ui.h"

#include "catch2/catch_test_macros.hpp"

#include "eu/tests/approx.h"

using namespace eu;
using namespace eu::tests;
using namespace eu::core;


TEST_CASE("ui-region-works", "[ui]")
{
	// todo(Gustav): expand tests
	CHECK(custom::Approx(v2{ 0, 0 }) == calculate_region({0, 0}, {0, 0}, {0, 0}, {10, 10}, 10));
}
