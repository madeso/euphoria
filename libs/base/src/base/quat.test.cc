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


using namespace eu::convert;
using namespace eu::tests;
using namespace eu;

TEST_CASE("quat-identity", "[quat]")
{
	const auto q = q_identity;
	CHECK(q == approx(Q(1, v3(0, 0, 0))));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
	CHECK(Q(4, v3(1, 2, 3)).get_vec_part() == approx(v3(1, 2, 3)));
}

#define EXPECT_PRED_FORMAT2(a, b) CHECK(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
	SECTION("identity")
	{
		const auto q = q_identity;
		CHECK(q.get_local_right() == approx(kk::right));
		CHECK(q.get_local_up() == approx(kk::up));
		CHECK(q.get_local_in() == approx(kk::in));
	}

	SECTION("look left")
	{
		const auto q = Q::from(rha(kk::up, 90_deg));
		CHECK(q.get_local_right() == approx(kk::in));
		CHECK(q.get_local_up() == approx(kk::up));
		CHECK(q.get_local_in() == approx(kk::left));
	}

	SECTION("look up")
	{
		const auto q = Q::from(rha(kk::right, 90_deg));
		CHECK(q.get_local_right() == approx(kk::right));
		CHECK(q.get_local_up() == approx(kk::out));
		CHECK(q.get_local_in() == approx(kk::up));
	}

	SECTION("roll right")
	{
		const auto q = Q::from(rha(kk::in, 90_deg));
		CHECK(q.get_local_right() == approx(kk::down));
		CHECK(q.get_local_up() == approx(kk::right));
		CHECK(q.get_local_in() == approx(kk::in));
	}
}


TEST_CASE("quat-testLook", "[quat]")
{
	CHECK(Q::look_at(v3(0, 0, 0), v3(5, 0, 0), kk::up ) == approx(Q::from (rha(kk::up, -90_deg))));
	CHECK(Q::look_at(v3(0, 0, 0), v3(2, 0, 0), kk::up) == approx(Q::from(rha(kk::up, -90_deg))));
	CHECK(Q::look_at(v3(0, 0, 0), v3(0.25f, 0, 0), kk::up) == approx(Q::from(rha(kk::up, -90_deg))));

	// need to pass in a normalized vec3 or it won't compile, hence the
	// .get_normalized() call
	// Z looks reversed, but remember, negative direction is in
	CHECK(Q::look_in_direction(n3(0, 0, -1), kk::up) == approx(Q::from(rha(kk::up, no_rotation))));

    // CHECK(Q::look_in_direction(n3(0, 0, 1), kk::up)
	// 	== approx(Q::from(rha(kk::up, 180_deg))));

	CHECK(Q::look_in_direction(n3(1, 0, 0), kk::up) == approx(Q::from(rha(kk::up, -90_deg))));
	CHECK(Q::look_in_direction(n3(-1, 0, 0), kk::up) == approx(Q::from(rha(kk::up, 90_deg))));

	// todo(Gustav): add more test where up != up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
	CHECK(q_identity
		.then_get_rotated(Q::from(rha(kk::up, 90_deg)))
				== approx(Q::from(rha(kk::up, 90_deg))));
	CHECK(Q::from(rha(kk::up, 90_deg))
		.then_get_rotated(q_identity)
				== approx(Q::from(rha(kk::up, 90_deg))));

	CHECK(Q::from(rha(kk::right, 90_deg))
		.then_get_rotated(Q::from(rha(kk::up, -90_deg)))
		== approx(Q::from(rha(kk::up, -90_deg))
				.then_get_rotated(Q::from(rha(kk::out, 90_deg)))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
	CHECK(rha(kk::up, no_rotation) == approx(rha(kk::up, no_rotation)));
	CHECK(rha(kk::right, 90_deg) == approx(rha(kk::right, 90_deg)));
	CHECK(rha(-kk::up, 45_deg) == approx(rha(kk::up, -45_deg)));
	CHECK(rha(-kk::right, -90_deg) == approx(rha(kk::right, 90_deg)));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
	CHECK(std::nullopt == AA::from(Q::from(rha(kk::up, no_rotation))));
	CHECK(std::nullopt == AA::from(Q::from(rha(kk::right, no_rotation))));

	CHECK(AA::from(Q::from(rha(kk::right, 90_deg))) == approx(rha(kk::right, 90_deg)));
	CHECK(AA::from(Q::from(rha(-kk::up, 45_deg))) == approx(rha(kk::up, -45_deg)));
	CHECK(AA::from(Q::from(rha(-kk::right, -90_deg))) == approx(rha(kk::right, 90_deg)));
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
	const auto a = Q::from(rha(kk::up, 90_deg));
	const auto b = Q::from(rha(kk::up, -90_deg)).get_conjugate();
	CHECK(b == approx(a));
}

TEST_CASE("quat-print", "[quat]")
{
	const auto q = Q(1, v3(2, 3, 4));

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
	CHECK(Q::from(Ypr{0_deg, 90_deg, 0_deg}) == approx(Q::from(rha(kk::up, 90_deg))));
}

TEST_CASE("quat-from-to", "[quat]")
{
	const auto q1 = Q::from(rha(kk::up, 0_deg));
	const auto q2 = Q::from(rha(kk::up, 90_deg));
	const auto q = Q::from_to(q1, q2);
	CHECK(q == approx(Q::from(rha(kk::up, 90_deg))));
}

TEST_CASE("quat-get-inverse", "[quat]")
{
	const auto q = Q::from(rha(kk::up, 90_deg));
	const auto inv = q.get_inverse();
	CHECK(inv == approx(Q::from(rha(kk::up, -90_deg))));
}

TEST_CASE("quat-get-negated", "[quat]")
{
	const auto q = Q(1, v3(2, 3, 4));
	const auto neg = q.get_negated();
	CHECK(neg == approx(Q(-1, v3(-2, -3, -4))));
}

TEST_CASE("quat-nlerp-n-slerp", "[quat]")
{
	const auto from = Q::from(rha(kk::up, 0_deg));
	const auto to = Q::from(rha(kk::up, 90_deg));

    // todo(Gustav): provide actual nlerp values for 25% and 75%
    const auto q25 = Q::from(rha(kk::up, 22.5_deg));
    const auto q50 = Q::from(rha(kk::up, 45_deg));
    const auto q75 = Q::from(rha(kk::up, 67.5_deg));

    SECTION("nlerp")
    {
        CHECK(Q::nlerp(from, 0.0f, to) == approx(from));
	    CHECK(Q::nlerp(from, 0.25f, to) != approx(q25));
	    CHECK(Q::nlerp(from, 0.5f, to) == approx(q50));
	    CHECK(Q::nlerp(from, 0.75f, to) != approx(q75));
        CHECK(Q::nlerp(from, 1.0f, to) == approx(to));
    }


    SECTION("slerp")
    {
        CHECK(Q::slerp(from, 0.0f, to) == approx(from));
	    CHECK(Q::slerp(from, 0.25f, to) == approx(q25));
	    CHECK(Q::slerp(from, 0.5f, to) == approx(q50));
	    CHECK(Q::slerp(from, 0.75f, to) == approx(q75));
        CHECK(Q::slerp(from, 1.0f, to) == approx(to));
    }
}

TEST_CASE("quat-operators", "[quat]")
{
	const auto q1 = Q(1, v3(2, 3, 4));
	const auto q2 = Q(0.5f, v3(1, 1.5f, 2));


	SECTION("multiplication with scalar")
	{
		const auto result = q1 * 2.0f;
		CHECK(result == approx(Q(2, v3(4, 6, 8))));
	}

	SECTION("multiplication with scalar (reversed)")
	{
		const auto result = 2.0f * q1;
		CHECK(result == approx(Q(2, v3(4, 6, 8))));
	}

	SECTION("multiplication with quaternion")
	{
		CHECK(q1 * q2 == approx(Q(-14.0f, v3(2, 3, 4))));
        CHECK(q2 * q1 == approx(Q(-14.0f, v3(2, 3, 4))));
	}
}

TEST_CASE("quat-get-local", "[quat]")
{
	SECTION("identity")
	{
		const auto q = q_identity;

		CHECK(q.get_local_right() == approx(kk::right));
		CHECK(q.get_local_left() == approx(kk::left));
		CHECK(q.get_local_up() == approx(kk::up));
		CHECK(q.get_local_down() == approx(kk::down));
		CHECK(q.get_local_in() == approx(kk::in));
		CHECK(q.get_local_out() == approx(kk::out));
	}

	SECTION("rotated 90 degrees around up")
	{
		const auto q = Q::from(rha(kk::up, 90_deg));

		CHECK(q.get_local_right() == approx(kk::in));
		CHECK(q.get_local_left() == approx(kk::out));
		CHECK(q.get_local_up() == approx(kk::up));
		CHECK(q.get_local_down() == approx(kk::down));
		CHECK(q.get_local_in() == approx(kk::left));
		CHECK(q.get_local_out() == approx(kk::right));
	}

	SECTION("rotated 90 degrees around right")
	{
		const auto q = Q::from(rha(kk::right, 90_deg));

		CHECK(q.get_local_right() == approx(kk::right));
		CHECK(q.get_local_left() == approx(kk::left));
		CHECK(q.get_local_up() == approx(kk::out));
		CHECK(q.get_local_down() == approx(kk::in));
		CHECK(q.get_local_in() == approx(kk::up));
		CHECK(q.get_local_out() == approx(kk::down));
	}

	SECTION("rotated 90 degrees around in")
	{
		const auto q = Q::from(rha(kk::in, 90_deg));

		CHECK(q.get_local_right() == approx(kk::down));
		CHECK(q.get_local_left() == approx(kk::up));
		CHECK(q.get_local_up() == approx(kk::right));
		CHECK(q.get_local_down() == approx(kk::left));
		CHECK(q.get_local_in() == approx(kk::in));
		CHECK(q.get_local_out() == approx(kk::out));
	}
}
