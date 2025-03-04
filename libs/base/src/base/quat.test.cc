#include "base/quat.h"

#include "tests/approx_equal.h"
#include "base/axisangle.h"

#include "catch2/catch_all.hpp"

// move to a common file in test lib
namespace Catch
{
	template <typename T>
	struct StringMaker<std::optional<T>>
	{
		static std::string convert(std::optional<T> const& value)
		{
			if (!value)
				{ return "<empty>"; }

			return fmt::to_string(*value);
		}
	};

	template <>
	struct StringMaker<std::nullopt_t>
	{
		static std::string convert(std::nullopt_t const&)
		{
			return "<empty>";
		}
	};
}


using namespace eu::tests;

TEST_CASE("quat-identity", "[quat]")
{
	const auto q = eu::q_identity;
	REQUIRE(q == approx(eu::Q(1, eu::v3(0, 0, 0))));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
	REQUIRE(eu::Q(4, eu::v3(1, 2, 3)).get_vec_part() == approx(eu::v3(1, 2, 3)));
}

#define EXPECT_PRED_FORMAT2(a, b) CHECK(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
	SECTION("identity")
	{
		const auto q = eu::q_identity;
		REQUIRE(q.get_local_right() == approx(eu::kk::right));
		REQUIRE(q.get_local_up() == approx(eu::kk::up));
		REQUIRE(q.get_local_in() == approx(eu::kk::in));
	}

	SECTION("look left")
	{
		const auto q = eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(90)));
		REQUIRE(q.get_local_right() == approx(eu::kk::in));
		REQUIRE(q.get_local_up() == approx(eu::kk::up));
		REQUIRE(q.get_local_in() == approx(eu::kk::left));
	}

	SECTION("look up")
	{
		const auto q = eu::Q::from(eu::rha(eu::kk::right, eu::An::from_degrees(90)));
		REQUIRE(q.get_local_right() == approx(eu::kk::right));
		REQUIRE(q.get_local_up() == approx(eu::kk::out));
		REQUIRE(q.get_local_in() == approx(eu::kk::up));
	}

	SECTION("roll right")
	{
		const auto q = eu::Q::from(eu::rha(eu::kk::in, eu::An::from_degrees(90)));
		REQUIRE(q.get_local_right() == approx(eu::kk::down));
		REQUIRE(q.get_local_up() == approx(eu::kk::right));
		REQUIRE(q.get_local_in() == approx(eu::kk::in));
	}
}


TEST_CASE("quat-testLook", "[quat]")
{
	CHECK(eu::Q::look_at(eu::v3(0, 0, 0), eu::v3(5, 0, 0), eu::kk::up )
		== approx(eu::Q::from (eu::rha(eu::kk::up, eu::An::from_degrees(-90)))));
	CHECK(eu::Q::look_at(eu::v3(0, 0, 0), eu::v3(2, 0, 0), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(-90)))));
	CHECK(eu::Q::look_at(eu::v3(0, 0, 0), eu::v3(0.25f, 0, 0), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(-90)))));

	// need to pass in a normalized vec3 or it won't compile, hence the
	// .get_normalized() call
	// Z looks reversed, but remember, negative direction is in
	CHECK(eu::Q::look_in_direction(eu::n3(0, 0, -1), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, eu::no_rotation))));
	CHECK(eu::Q::look_in_direction(eu::n3(0, 0, 1), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(180)))));
	CHECK(eu::Q::look_in_direction(eu::n3(1, 0, 0), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(-90)))));
	CHECK(eu::Q::look_in_direction(eu::n3(-1, 0, 0), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(90)))));

	// todo(Gustav): add more test where up != up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
	CHECK(eu::q_identity
		.then_get_rotated(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(90))))
				== approx(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(90)))));
	CHECK(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(90)))
		.then_get_rotated(eu::q_identity)
				== approx(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(90)))));
	
	CHECK(eu::q_identity
		.then_get_rotated(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(90))))
				== approx(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(90)))));

	CHECK(eu::Q::from(eu::rha(eu::kk::right, eu::An::from_degrees(90)))
		.then_get_rotated(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(-90))))
		== approx(eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(-90)))
				.then_get_rotated(eu::Q::from(eu::rha(eu::kk::out, eu::An::from_degrees(90))))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
	CHECK(eu::rha(eu::kk::up, eu::no_rotation)
		== approx(eu::rha(eu::kk::up, eu::no_rotation)));
	CHECK(eu::rha(eu::kk::right, eu::An::from_degrees(90))
		== approx(eu::rha(eu::kk::right, eu::An::from_degrees(90))));
	CHECK(eu::rha(-eu::kk::up, eu::An::from_degrees(45))
		== approx(eu::rha(eu::kk::up, eu::An::from_degrees(-45))));
	CHECK(eu::rha(-eu::kk::right, eu::An::from_degrees(-90))
		== approx(eu::rha(eu::kk::right, eu::An::from_degrees(90))));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
	CHECK(std::nullopt == eu::AA::from(eu::Q::from(eu::rha(eu::kk::up, eu::no_rotation))));
	CHECK(std::nullopt == eu::AA::from(eu::Q::from(eu::rha(eu::kk::right, eu::no_rotation))));

	CHECK(eu::AA::from(eu::Q::from(eu::rha(eu::kk::right, eu::An::from_degrees(90))))
		== approx(eu::rha(eu::kk::right, eu::An::from_degrees(90))));
	CHECK(eu::AA::from(eu::Q::from(eu::rha(-eu::kk::up, eu::An::from_degrees(45))))
		== approx(eu::rha(eu::kk::up, eu::An::from_degrees(-45))));
	CHECK(eu::AA::from(eu::Q::from(eu::rha(-eu::kk::right, eu::An::from_degrees(-90))))
		== approx(eu::rha(eu::kk::right, eu::An::from_degrees(90))));
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
	const auto a = eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(90)));
	const auto b = eu::Q::from(eu::rha(eu::kk::up, eu::An::from_degrees(-90))).get_conjugate();
	REQUIRE(b == approx(a));
}
