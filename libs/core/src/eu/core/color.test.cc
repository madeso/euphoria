#include "eu/core/color.h"

#include "catch2/catch_test_macros.hpp"

using namespace eu::core;

TEST_CASE("texture_color", "[color]")
{
	CHECK(color_from_rgba(0xFF, 0xFF, 0xFF, 0xFF) == static_cast<SingleColor>(0xFFFFFFFF));

	CHECK(color_from_rgba(0xFF, 0x00, 0x00, 0x00) == static_cast<SingleColor>(0x000000FF));
	CHECK(color_from_rgba(0x00, 0xFF, 0x00, 0x00) == static_cast<SingleColor>(0x0000FF00));
	CHECK(color_from_rgba(0x00, 0x00, 0xFF, 0x00) == static_cast<SingleColor>(0x00FF0000));
	CHECK(color_from_rgba(0x00, 0x00, 0x00, 0xFF) == static_cast<SingleColor>(0xFF000000));
}
