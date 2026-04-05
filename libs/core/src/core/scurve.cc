#include "core/scurve.h"

#include "assert/assert.h"
#include "base/cint.h"

#include <cmath>

namespace eu::core
{

float square(float x)
{
	return x * x;
}

SCurve s_curve_from_input(float mx, float my)
{
	const auto p0 = mx;
	// gustav addition: switch p1 to match curve to xy
	const auto p1 = mx > 0.5f ? my : 1 - my;

	const auto o = [](float x)
	{
		return x < 0.5f ? -0.5f * (2.0f * x * (2.0f * x - 2.0f)) : 2.0f * (square(x - 0.5f)) + 0.5f;
	};
	const auto q0 = o(p0);
	const auto q1 = o(p1);

	const auto a = [](float x)
	{
		return std::pow(3.0f, x);
	};
	const auto slope = a(10 * (q0 - 0.5f));
	const auto threshold = 0.5f + (0.5f * (1.0f - q1));
	return {slope, threshold};
}

float calculate_s_curve(float x, float slope, float threshold)
{
	constexpr float machine_epsilon = 0.00001f;
	return x < threshold ?
		threshold * x / (x + slope * (threshold - x) + machine_epsilon) :
		((1 - threshold) * (x - 1)) / (1 - x - slope * (threshold - x) + machine_epsilon) + 1
	;
}

}  //  namespace eu::core
