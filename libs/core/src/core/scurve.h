#pragma once

namespace eu::core
{

/** \addtogroup scurcve S-Curve
 * \brief A tweakable curve similar to easing functions.
 * Discovered by Yann van der Cruyssen/Morusque on [twitter](https://x.com/Morusque/status/1352569197499441155).
 * From [A Convenient Generalization of Schlick’s Bias and Gain Functions (pdf)](https://arxiv.org/pdf/2010.09714) by Jonathan T. Barron.
 *  @{
*/

/// Contains the parameters for an S-Curve.
struct SCurve
{
	float slope = 1.0f;	 ///< >=0
	float threshold = 0.75f;  ///< [0,1]

	constexpr SCurve() = default;

	constexpr SCurve(float s, float t)
		: slope(s)
		, threshold(t)
	{}

    // todo(Gustav): give a better name
	constexpr static SCurve light_curve()
	{
		return {5.0f, 1.0f};
	}
};

/// Generate an S-Curve from user input.
/// Adapted from [Colugo's](https://twitter.com/ColugoMusic/status/1363071439679729665?s=20) S curve editor implemented in/on [desmos](https://www.desmos.com/calculator/ibek4vkdiw)
/// @param x [0,1] 0=left, 1=right
/// @param y [0,1] 0=bottom, 1=top
SCurve s_curve_from_input(float x, float y);

/// Calculates an S-Curve.
/// this takes floats instead of the curve since that means it could more easily be copied to glsl
/// @param x [0,1]
/// @param slope >=0
/// @param threshold [0,1]
float calculate_s_curve(float x, float slope, float threshold);

/**
 * @}
*/

}  //  namespace klotter
