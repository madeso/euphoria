#include "base/quat.h"

#include "tests/approx_equal.h"
#include "base/axisangle.h"

#include "catch2/catch_all.hpp"

using namespace eu::convert;

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
	CHECK(q == approx(eu::Q(1, eu::v3(0, 0, 0))));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
	CHECK(eu::Q(4, eu::v3(1, 2, 3)).get_vec_part() == approx(eu::v3(1, 2, 3)));
}

#define EXPECT_PRED_FORMAT2(a, b) CHECK(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
	SECTION("identity")
	{
		const auto q = eu::q_identity;
		CHECK(q.get_local_right() == approx(eu::kk::right));
		CHECK(q.get_local_up() == approx(eu::kk::up));
		CHECK(q.get_local_in() == approx(eu::kk::in));
	}

	SECTION("look left")
	{
		const auto q = eu::Q::from(eu::rha(eu::kk::up, 90_deg));
		CHECK(q.get_local_right() == approx(eu::kk::in));
		CHECK(q.get_local_up() == approx(eu::kk::up));
		CHECK(q.get_local_in() == approx(eu::kk::left));
	}

	SECTION("look up")
	{
		const auto q = eu::Q::from(eu::rha(eu::kk::right, 90_deg));
		CHECK(q.get_local_right() == approx(eu::kk::right));
		CHECK(q.get_local_up() == approx(eu::kk::out));
		CHECK(q.get_local_in() == approx(eu::kk::up));
	}

	SECTION("roll right")
	{
		const auto q = eu::Q::from(eu::rha(eu::kk::in, 90_deg));
		CHECK(q.get_local_right() == approx(eu::kk::down));
		CHECK(q.get_local_up() == approx(eu::kk::right));
		CHECK(q.get_local_in() == approx(eu::kk::in));
	}
}


TEST_CASE("quat-testLook", "[quat]")
{
	CHECK(eu::Q::look_at(eu::v3(0, 0, 0), eu::v3(5, 0, 0), eu::kk::up )
		== approx(eu::Q::from (eu::rha(eu::kk::up, -90_deg))));
	CHECK(eu::Q::look_at(eu::v3(0, 0, 0), eu::v3(2, 0, 0), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, -90_deg))));
	CHECK(eu::Q::look_at(eu::v3(0, 0, 0), eu::v3(0.25f, 0, 0), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, -90_deg))));

	// need to pass in a normalized vec3 or it won't compile, hence the
	// .get_normalized() call
	// Z looks reversed, but remember, negative direction is in
	CHECK(eu::Q::look_in_direction(eu::n3(0, 0, -1), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, eu::no_rotation))));

    // CHECK(eu::Q::look_in_direction(eu::n3(0, 0, 1), eu::kk::up)
	// 	== approx(eu::Q::from(eu::rha(eu::kk::up, 180_deg))));

	CHECK(eu::Q::look_in_direction(eu::n3(1, 0, 0), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, -90_deg))));
	CHECK(eu::Q::look_in_direction(eu::n3(-1, 0, 0), eu::kk::up)
		== approx(eu::Q::from(eu::rha(eu::kk::up, 90_deg))));

	// todo(Gustav): add more test where up != up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
	CHECK(eu::q_identity
		.then_get_rotated(eu::Q::from(eu::rha(eu::kk::up, 90_deg)))
				== approx(eu::Q::from(eu::rha(eu::kk::up, 90_deg))));
	CHECK(eu::Q::from(eu::rha(eu::kk::up, 90_deg))
		.then_get_rotated(eu::q_identity)
				== approx(eu::Q::from(eu::rha(eu::kk::up, 90_deg))));
	
	CHECK(eu::q_identity
		.then_get_rotated(eu::Q::from(eu::rha(eu::kk::up, 90_deg)))
				== approx(eu::Q::from(eu::rha(eu::kk::up, 90_deg))));

	CHECK(eu::Q::from(eu::rha(eu::kk::right, 90_deg))
		.then_get_rotated(eu::Q::from(eu::rha(eu::kk::up, -90_deg)))
		== approx(eu::Q::from(eu::rha(eu::kk::up, -90_deg))
				.then_get_rotated(eu::Q::from(eu::rha(eu::kk::out, 90_deg)))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
	CHECK(eu::rha(eu::kk::up, eu::no_rotation)
		== approx(eu::rha(eu::kk::up, eu::no_rotation)));
	CHECK(eu::rha(eu::kk::right, 90_deg)
		== approx(eu::rha(eu::kk::right, 90_deg)));
	CHECK(eu::rha(-eu::kk::up, 45_deg)
		== approx(eu::rha(eu::kk::up, -45_deg)));
	CHECK(eu::rha(-eu::kk::right, -90_deg)
		== approx(eu::rha(eu::kk::right, 90_deg)));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
	CHECK(std::nullopt == eu::AA::from(eu::Q::from(eu::rha(eu::kk::up, eu::no_rotation))));
	CHECK(std::nullopt == eu::AA::from(eu::Q::from(eu::rha(eu::kk::right, eu::no_rotation))));

	CHECK(eu::AA::from(eu::Q::from(eu::rha(eu::kk::right, 90_deg)))
		== approx(eu::rha(eu::kk::right, 90_deg)));
	CHECK(eu::AA::from(eu::Q::from(eu::rha(-eu::kk::up, 45_deg)))
		== approx(eu::rha(eu::kk::up, -45_deg)));
	CHECK(eu::AA::from(eu::Q::from(eu::rha(-eu::kk::right, -90_deg)))
		== approx(eu::rha(eu::kk::right, 90_deg)));
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
	const auto a = eu::Q::from(eu::rha(eu::kk::up, 90_deg));
	const auto b = eu::Q::from(eu::rha(eu::kk::up, -90_deg)).get_conjugate();
	CHECK(b == approx(a));
}

TEST_CASE("quat-print", "[quat]")
{
	const auto q = eu::Q(1, eu::v3(2, 3, 4));

	SECTION("fmt")
	{
		std::string str = fmt::format("{0}", q);
		CHECK(str == "(1, (2, 3, 4))");
	}

	SECTION("stream")
	{
		std::ostringstream oss;
		oss << q;
		CHECK(oss.str() == "(1, (2, 3, 4))");
	}
}

TEST_CASE("quat-from-ypr", "[quat]")
{
	CHECK(eu::Q::from(eu::Ypr{0_deg, 90_deg, 0_deg}) == approx(eu::Q::from(eu::rha(eu::kk::up, 90_deg))));
}

TEST_CASE("quat-from-to", "[quat]")
{
	const auto q1 = eu::Q::from(eu::rha(eu::kk::up, 0_deg));
	const auto q2 = eu::Q::from(eu::rha(eu::kk::up, 90_deg));
	const auto q = eu::Q::from_to(q1, q2);
	CHECK(q == approx(eu::Q::from(eu::rha(eu::kk::up, 90_deg))));
}

TEST_CASE("quat-get-inverse", "[quat]")
{
	const auto q = eu::Q::from(eu::rha(eu::kk::up, 90_deg));
	const auto inv = q.get_inverse();
	CHECK(inv == approx(eu::Q::from(eu::rha(eu::kk::up, -90_deg))));
}

TEST_CASE("quat-get-negated", "[quat]")
{
	const auto q = eu::Q(1, eu::v3(2, 3, 4));
	const auto neg = q.get_negated();
	CHECK(neg == approx(eu::Q(-1, eu::v3(-2, -3, -4))));
}

TEST_CASE("quat-nlerp-n-slerp", "[quat]")
{
	const auto from = eu::Q::from(eu::rha(eu::kk::up, 0_deg));
	const auto to = eu::Q::from(eu::rha(eu::kk::up, 90_deg));

    // todo(Gustav): provide actual nlerp values for 25% and 75%
    const auto q25 = eu::Q::from(eu::rha(eu::kk::up, 22.5_deg));
    const auto q50 = eu::Q::from(eu::rha(eu::kk::up, 45_deg));
    const auto q75 = eu::Q::from(eu::rha(eu::kk::up, 67.5_deg));

    SECTION("nlerp")
    {
        CHECK(eu::Q::nlerp(from, 0.0f, to) == approx(from));
	    CHECK(eu::Q::nlerp(from, 0.25f, to) != approx(q25));
	    CHECK(eu::Q::nlerp(from, 0.5f, to) == approx(q50));
	    CHECK(eu::Q::nlerp(from, 0.75f, to) != approx(q75));
        CHECK(eu::Q::nlerp(from, 1.0f, to) == approx(to));
    }


    SECTION("slerp")
    {
        CHECK(eu::Q::slerp(from, 0.0f, to) == approx(from));
	    CHECK(eu::Q::slerp(from, 0.25f, to) == approx(q25));
	    CHECK(eu::Q::slerp(from, 0.5f, to) == approx(q50));
	    CHECK(eu::Q::slerp(from, 0.75f, to) == approx(q75));
        CHECK(eu::Q::slerp(from, 1.0f, to) == approx(to));
    }
}

TEST_CASE("quat-operators", "[quat]")
{
	const auto q1 = eu::Q(1, eu::v3(2, 3, 4));
	const auto q2 = eu::Q(0.5f, eu::v3(1, 1.5f, 2));


	SECTION("multiplication with scalar")
	{
		const auto result = q1 * 2.0f;
		CHECK(result == approx(eu::Q(2, eu::v3(4, 6, 8))));
	}

	SECTION("multiplication with scalar (reversed)")
	{
		const auto result = 2.0f * q1;
		CHECK(result == approx(eu::Q(2, eu::v3(4, 6, 8))));
	}

	SECTION("multiplication with quaternion")
	{
		CHECK(q1 * q2 == approx(eu::Q(-14.0f, eu::v3(2, 3, 4))));
        CHECK(q2 * q1 == approx(eu::Q(-14.0f, eu::v3(2, 3, 4))));
	}
}
