#include "base/colors.h"

#include <algorithm>
#include <cmath>

#include "base/numeric.h"

namespace eu
{

/// converts from gamma space to linear space
float linear_from_srgb(float value, float gamma)
{
	// aka srgb_transfer_function_inv
	

	// todo(Gustav): is this srgb or a basic gamma2 transformation? what's the difference? verify all code usage!
	return std::pow(value, gamma);
}


Lin_rgb linear_from_srgb(const Rgb& value, float gamma)
{
	return
	{
        .r = linear_from_srgb(value.r, gamma),
        .g = linear_from_srgb(value.g, gamma),
        .b = linear_from_srgb(value.b, gamma)
	};
}


float linear_from_srgb(float a)
{
	return .04045f < a ? powf((a + .055f) / 1.055f, 2.4f) : a / 12.92f;
}

Lin_rgb linear_from_srgb(const Rgb& value)
{
	return {.r = linear_from_srgb(value.r), .g = linear_from_srgb(value.g), .b = linear_from_srgb(value.b)};
}



float srgb_from_linear(float a)
{
	return .0031308f >= a ? 12.92f * a : 1.055f * powf(a, .4166666666666667f) - .055f;
}


Rgb srgb_from_linear(const Lin_rgb& value)
{
	return {srgb_from_linear(value.r), srgb_from_linear(value.g), srgb_from_linear(value.b)};
}

struct MaxSaturationCoefficients
{
	float k0;
	float k1;
	float k2;
	float k3;
	float k4;
	float wl;
	float wm;
	float ws;
};

// Finds the maximum saturation possible for a given hue that fits in sRGB
// Saturation here is defined as S = C/L
// a and b must be normalized so a^2 + b^2 == 1
float compute_max_saturation(float a, float b)
{
	// Max saturation will be when one of r, g or b goes below zero.

	// Select different coefficients depending on which component goes below zero first
	const auto c = ([&]() -> MaxSaturationCoefficients {
		if (-1.88170328f * a - 0.80936493f * b > 1)
		{
			// Red component
			return
			{
				.k0 = +1.19086277f,
				.k1 = +1.76576728f,
				.k2 = +0.59662641f,
				.k3 = +0.75515197f,
				.k4 = +0.56771245f,
				.wl = +4.0767416621f,
				.wm = -3.3077115913f,
				.ws = +0.2309699292f,
			};
		}
		else if (1.81444104f * a - 1.19445276f * b > 1)
		{
			// Green component
			return
			{
				.k0 = +0.73956515f,
				.k1 = -0.45954404f,
				.k2 = +0.08285427f,
				.k3 = +0.12541070f,
				.k4 = +0.14503204f,
				.wl = -1.2684380046f,
				.wm = +2.6097574011f,
				.ws = -0.3413193965f,
			};
		}
		else
		{
			// Blue component
			return
			{
				.k0 = +1.35733652f,
				.k1 = -0.00915799f,
				.k2 = -1.15130210f,
				.k3 = -0.50559606f,
				.k4 = +0.00692167f,
				.wl = -0.0041960863f,
				.wm = -0.7034186147f,
				.ws = +1.7076147010f,
			};
		}
	})();

	// Approximate max saturation using a polynomial:
	float big_s = c.k0 + c.k1 * a + c.k2 * b + c.k3 * a * a + c.k4 * a * b;

	// Do one step Halley's method to get closer
	// this gives an error less than 10e6, except for some blue hues where the dS/dh is close to infinite
	// this should be sufficient for most applications, otherwise do two/three steps

	const auto k_l = +0.3963377774f * a + 0.2158037573f * b;
	const auto k_m = -0.1055613458f * a - 0.0638541728f * b;
	const auto k_s = -0.0894841775f * a - 1.2914855480f * b;

	{
		const auto l_prim = 1.f + big_s * k_l;
		const auto m_prim = 1.f + big_s * k_m;
		const auto s_prim = 1.f + big_s * k_s;

		const auto l = l_prim * l_prim * l_prim;
		const auto m = m_prim * m_prim * m_prim;
		const auto s = s_prim * s_prim * s_prim;

		const auto l_d_big_s = 3.f * k_l * l_prim * l_prim;
		const auto m_d_big_s = 3.f * k_m * m_prim * m_prim;
		const auto s_d_big_s = 3.f * k_s * s_prim * s_prim;

		const auto l_d_big_s2 = 6.f * k_l * k_l * l_prim;
		const auto m_d_big_s2 = 6.f * k_m * k_m * m_prim;
		const auto s_d_big_s2 = 6.f * k_s * k_s * s_prim;

		const auto f = c.wl * l + c.wm * m + c.ws * s;
		const auto f1 = c.wl * l_d_big_s + c.wm * m_d_big_s + c.ws * s_d_big_s;
		const auto f2 = c.wl * l_d_big_s2 + c.wm * m_d_big_s2 + c.ws * s_d_big_s2;

		big_s = big_s - f * f1 / (f1 * f1 - 0.5f * f * f2);
	}

	return big_s;
}


struct Lc
{
	float l;
	float c;
};


/// finds L_cusp and C_cusp for a given hue
/// a and b must be normalized so a^2 + b^2 == 1
Lc find_cusp(float a, float b)
{
	// First, find the maximum saturation (saturation S = C/L)
	const auto big_s_cusp = compute_max_saturation(a, b);

	// Convert to linear sRGB to find the first point where at least one of r,g or b >= 1:
	const auto rgb_at_max = linear_from_oklab({.l = 1, .a = big_s_cusp * a, .b = big_s_cusp * b});
	const auto big_l_cusp = cbrtf(1.f / std::max({rgb_at_max.r, rgb_at_max.g, rgb_at_max.b}));
	const auto big_c_cusp = big_l_cusp * big_s_cusp;

	return {.l = big_l_cusp, .c = big_c_cusp};
}


/// Finds intersection of the line defined by
/// `L = L0 * (1 - t) + t * L1;`
/// `C = t * C1;`
/// a and b must be normalized so a^2 + b^2 == 1
float find_gamut_intersection(float a, float b, float big_l1, float big_c1, float big_l0)
{
	// Find the cusp of the gamut triangle
	const auto cusp = find_cusp(a, b);

	// Find the intersection for upper and lower half separately
	if (((big_l1 - big_l0) * cusp.c - (cusp.l - big_l0) * big_c1) <= 0.f)
	{
		// Lower half
		return cusp.c * big_l0 / (big_c1 * cusp.l + cusp.c * (big_l0 - big_l1));
	}

	// Upper half

	// First intersect with triangle
	float t = cusp.c * (big_l0 - 1.f) / (big_c1 * (cusp.l - 1.f) + cusp.c * (big_l0 - big_l1));

	// Then one-step Halley's method
	const auto d_big_l = big_l1 - big_l0;
	const auto d_big_c = big_c1;

	const auto k_l = +0.3963377774f * a + 0.2158037573f * b;
	const auto k_m = -0.1055613458f * a - 0.0638541728f * b;
	const auto k_s = -0.0894841775f * a - 1.2914855480f * b;

	const auto l_dt = d_big_l + d_big_c * k_l;
	const auto m_dt = d_big_l + d_big_c * k_m;
	const auto s_dt = d_big_l + d_big_c * k_s;


	// If higher accuracy is required, 2 or 3 iterations of the following block can be used:
	{
		const auto big_l = big_l0 * (1.f - t) + t * big_l1;
		const auto big_c = t * big_c1;

		const auto l_prim = big_l + big_c * k_l;
		const auto m_prim = big_l + big_c * k_m;
		const auto s_prim = big_l + big_c * k_s;

		const auto l = l_prim * l_prim * l_prim;
		const auto m = m_prim * m_prim * m_prim;
		const auto s = s_prim * s_prim * s_prim;

		const auto ldt = 3 * l_dt * l_prim * l_prim;
		const auto mdt = 3 * m_dt * m_prim * m_prim;
		const auto sdt = 3 * s_dt * s_prim * s_prim;

		const auto ldt2 = 6 * l_dt * l_dt * l_prim;
		const auto mdt2 = 6 * m_dt * m_dt * m_prim;
		const auto sdt2 = 6 * s_dt * s_dt * s_prim;

		const auto r = 4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s - 1;
		const auto r1 = 4.0767416621f * ldt - 3.3077115913f * mdt + 0.2309699292f * sdt;
		const auto r2 = 4.0767416621f * ldt2 - 3.3077115913f * mdt2 + 0.2309699292f * sdt2;

		const auto u_r = r1 / (r1 * r1 - 0.5f * r * r2);
		auto t_r = -r * u_r;

		const auto g = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s - 1;
		const auto g1 = -1.2684380046f * ldt + 2.6097574011f * mdt - 0.3413193965f * sdt;
		const auto g2 = -1.2684380046f * ldt2 + 2.6097574011f * mdt2 - 0.3413193965f * sdt2;

		const auto u_g = g1 / (g1 * g1 - 0.5f * g * g2);
		auto t_g = -g * u_g;

		const auto b0 = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s - 1;
		const auto b1 = -0.0041960863f * ldt - 0.7034186147f * mdt + 1.7076147010f * sdt;
		const auto b2 = -0.0041960863f * ldt2 - 0.7034186147f * mdt2 + 1.7076147010f * sdt2;

		const auto u_b = b1 / (b1 * b1 - 0.5f * b0 * b2);
		auto t_b = -b0 * u_b;

		t_r = u_r >= 0.f ? t_r : FLT_MAX;
		t_g = u_g >= 0.f ? t_g : FLT_MAX;
		t_b = u_b >= 0.f ? t_b : FLT_MAX;

		t += std::min({t_r, t_g, t_b});
	}

	return t;
}


float clamp(float x, float min, float max)
{
	if (x < min)
	{
		return min;
	}

	if (x > max)
	{
		return max;
	}

	return x;
}


float sgn(float x)
{
	// todo(Gustav): make this clearer
	return static_cast<float>(0.f < x) - static_cast<float>(x < 0.f);
}


Lin_rgb gamut_clip_preserve_chroma(const Lin_rgb& rgb)
{
	if (rgb.r < 1 && rgb.g < 1 && rgb.b < 1 &&
		rgb.r > 0 && rgb.g > 0 && rgb.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto big_l = lab.l;
	const auto eps = 0.00001f;
	const auto big_c = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
	const auto a_prim = lab.a / big_c;
	const auto b_prim = lab.b / big_c;

	const auto big_l0 = clamp(big_l, 0, 1);

	const auto t = find_gamut_intersection(a_prim, b_prim, big_l, big_c, big_l0);
	const auto big_l_clipped = big_l0 * (1 - t) + t * big_l;
	const auto big_c_clipped = t * big_c;

	return linear_from_oklab({.l = big_l_clipped, .a = big_c_clipped * a_prim, .b = big_c_clipped * b_prim});
}


Lin_rgb gamut_clip_project_to_0_5(const Lin_rgb& rgb)
{
	if (rgb.r < 1 && rgb.g < 1 && rgb.b < 1 &&
		rgb.r > 0 && rgb.g > 0 && rgb.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto big_l = lab.l;
	const auto eps = 0.00001f;
	const auto big_c = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
	const auto a_prim = lab.a / big_c;
	const auto b_prim = lab.b / big_c;

	const auto big_l0 = 0.5f;

	const auto t = find_gamut_intersection(a_prim, b_prim, big_l, big_c, big_l0);
	const auto big_l_clipped = big_l0 * (1 - t) + t * big_l;
	const auto big_c_clipped = t * big_c;

	return linear_from_oklab({.l = big_l_clipped, .a = big_c_clipped * a_prim, .b = big_c_clipped * b_prim});
}


Lin_rgb gamut_clip_project_to_l_cusp(const Lin_rgb& rgb)
{
	if (rgb.r < 1 && rgb.g < 1 && rgb.b < 1 &&
		rgb.r > 0 && rgb.g > 0 && rgb.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto big_l = lab.l;
	const auto eps = 0.00001f;
	const auto big_c = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
	const auto a_prim = lab.a / big_c;
	const auto b_prim = lab.b / big_c;

	// The cusp is computed here and in find_gamut_intersection, an optimized solution would only compute it once.
	const auto cusp = find_cusp(a_prim, b_prim);

	const auto big_l0 = cusp.l;

	const auto t = find_gamut_intersection(a_prim, b_prim, big_l, big_c, big_l0);

	const auto big_l_clipped = big_l0 * (1 - t) + t * big_l;
	const auto big_c_clipped = t * big_c;

	return linear_from_oklab({.l = big_l_clipped, .a = big_c_clipped * a_prim, .b = big_c_clipped * b_prim});
}


Lin_rgb gamut_clip_adaptive_L0_0_5(const Lin_rgb& rgb, float alpha)
{
	if (rgb.r < 1 && rgb.g < 1 && rgb.b < 1 &&
		rgb.r > 0 && rgb.g > 0 && rgb.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto big_l = lab.l;
	const auto eps = 0.00001f;
	const auto big_c = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
	const auto a_prim = lab.a / big_c;
	const auto b_prim = lab.b / big_c;

	const auto big_ld = big_l - 0.5f;
	const auto e1 = 0.5f + std::abs(big_ld) + alpha * big_c;
	const auto big_l0 = 0.5f * (1.f + sgn(big_ld) * (e1 - std::sqrt(e1 * e1 - 2.f * std::abs(big_ld))));

	const auto t = find_gamut_intersection(a_prim, b_prim, big_l, big_c, big_l0);
	const auto big_l_clipped = big_l0 * (1.f - t) + t * big_l;
	const auto big_c_clipped = t * big_c;

	return linear_from_oklab({big_l_clipped, big_c_clipped * a_prim, big_c_clipped * b_prim});
}


Lin_rgb gamut_clip_adaptive_l0_l_cusp(const Lin_rgb& rgb, float alpha)
{
	if (rgb.r < 1 && rgb.g < 1 && rgb.b < 1 &&
		rgb.r > 0 && rgb.g > 0 && rgb.b > 0)
	{
		return rgb;
	}

	const auto lab = oklab_from_linear(rgb);

	const auto big_l = lab.l;
	const auto eps = 0.00001f;
	const auto big_c = std::max(eps, std::sqrt(lab.a * lab.a + lab.b * lab.b));
	const auto a_prim = lab.a / big_c;
	const auto b_prim = lab.b / big_c;

	// The cusp is computed here and in find_gamut_intersection, an optimized solution would only compute it once.
	const auto cusp = find_cusp(a_prim, b_prim);

	const auto big_ld = big_l - cusp.l;
	const auto k = 2.f * (big_ld > 0 ? 1.f - cusp.l : cusp.l);

	const auto e1 = 0.5f * k + std::abs(big_ld) + alpha * big_c / k;
	const auto big_l0 = cusp.l + 0.5f * (sgn(big_ld) * (e1 - std::sqrt(e1 * e1 - 2.f * k * std::abs(big_ld))));

	const auto t = find_gamut_intersection(a_prim, b_prim, big_l, big_c, big_l0);
	const auto big_l_clipped = big_l0 * (1.f - t) + t * big_l;
	const auto big_c_clipped = t * big_c;

	return linear_from_oklab({.l = big_l_clipped, .a = big_c_clipped * a_prim, .b = big_c_clipped * b_prim});
}


OkLab oklab_from_linear(const Lin_rgb& c)
{
	const auto cr = c.r;
	const auto cg = c.g;
	const auto cb = c.b;

	const auto l = 0.4122214708f * cr + 0.5363325363f * cg + 0.0514459929f * cb;
	const auto m = 0.2119034982f * cr + 0.6806995451f * cg + 0.1073969566f * cb;
	const auto s = 0.0883024619f * cr + 0.2817188376f * cg + 0.6299787005f * cb;

	const auto lp = std::cbrtf(l);
	const auto mp = std::cbrtf(m);
	const auto sp = std::cbrtf(s);

	return {
		.l = 0.2104542553f * lp + 0.7936177850f * mp - 0.0040720468f * sp,
		.a = 1.9779984951f * lp - 2.4285922050f * mp + 0.4505937099f * sp,
		.b = 0.0259040371f * lp + 0.7827717662f * mp - 0.8086757660f * sp,
	};
}

Lin_rgb linear_from_oklab(const OkLab& c)
{
	const auto lp = c.l + 0.3963377774f * c.a + 0.2158037573f * c.b;
	const auto mp = c.l - 0.1055613458f * c.a - 0.0638541728f * c.b;
	const auto sp = c.l - 0.0894841775f * c.a - 1.2914855480f * c.b;

	const auto l = lp * lp * lp;
	const auto m = mp * mp * mp;
	const auto s = sp * sp * sp;

	return {
        .r = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
        .g = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
        .b = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
	};
}

// https://en.wikipedia.org/wiki/Oklab_color_space#Conversion_to_and_from_Oklch
OkLch oklch_from_oklab(const OkLab& c)
{
	return {.l = c.l, .c = std::sqrt(c.a * c.a + c.b * c.b), .h = eu::atan2(c.b, c.a)};
}

OkLab oklab_from_oklch(const OkLch& c)
{
	return
	{
		.l = c.l, .a = c.c *eu::cos(c.h), .b = c.c *eu::sin(c.h)
	};
}


// Alternative representation of (L_cusp, C_cusp)
// Encoded so S = C_cusp/L_cusp and T = C_cusp/(1-L_cusp)
// The maximum value for C in the triangle is then found as std::min(S*L, T*(1-L)), for a given L
struct St
{
	float s;
	float t;
};

// toe function for L_r
float toe(float x)
{
	constexpr float k_1 = 0.206f;
	constexpr float k_2 = 0.03f;
	constexpr float k_3 = (1.f + k_1) / (1.f + k_2);
	return 0.5f * (k_3 * x - k_1 + std::sqrt((k_3 * x - k_1) * (k_3 * x - k_1) + 4 * k_2 * k_3 * x));
}

// inverse toe function for L_r
float toe_inv(float x)
{
	constexpr float k_1 = 0.206f;
	constexpr float k_2 = 0.03f;
	constexpr float k_3 = (1.f + k_1) / (1.f + k_2);
	return (x * x + k_1 * x) / (k_3 * (x + k_2));
}

St st_from_cusp(const Lc& cusp)
{
	const auto big_l = cusp.l;
	const auto big_c = cusp.c;
	return {.s = big_c / big_l, .t = big_c / (1 - big_l)};
}

Rgb srgb_from_okhsv(const OkHsv& hsv)
{
	const auto h = hsv.hue;
	const auto s = hsv.saturation;
	const auto v = hsv.value;

	const auto a_prim = eu::cos(h);
	const auto b_prim = eu::sin(h);

	const auto cusp = find_cusp(a_prim, b_prim);
	const auto big_st_max = st_from_cusp(cusp);
	const auto big_s_max = big_st_max.s;
	const auto big_t_max = big_st_max.t;
	const auto big_s_0 = 0.5f;
	const auto k = 1 - big_s_0 / big_s_max;

	// first we compute L and V as if the gamut is a perfect triangle:

	// L, C when v==1:
	const auto big_l_v = 1 - s * big_s_0 / (big_s_0 + big_t_max - big_t_max * k * s);
	const auto big_c_v = s * big_t_max * big_s_0 / (big_s_0 + big_t_max - big_t_max * k * s);

	auto big_l = v * big_l_v;
	auto big_c = v * big_c_v;

	// then we compensate for both toe and the curved top part of the triangle:
	const auto big_l_vt = toe_inv(big_l_v);
	const auto big_c_vt = big_c_v * big_l_vt / big_l_v;

	const auto big_l_new = toe_inv(big_l);
	big_c = big_c * big_l_new / big_l;
	big_l = big_l_new;

	const auto rgb_scale = linear_from_oklab({.l = big_l_vt, .a = a_prim * big_c_vt, .b = b_prim * big_c_vt});
	const auto scale_big_l = cbrtf(1.f / std::max({rgb_scale.r, rgb_scale.g, rgb_scale.b, 0.f}));

	big_l = big_l * scale_big_l;
	big_c = big_c * scale_big_l;

	const auto rgb = linear_from_oklab({big_l, big_c * a_prim, big_c * b_prim});
	return srgb_from_linear(rgb);
}

OkHsv okhsv_from_srgb(const Rgb& rgb)
{
	const auto lab = oklab_from_linear(linear_from_srgb(rgb));

	auto big_c = std::sqrt(lab.a * lab.a + lab.b * lab.b);
	const auto a_prim = lab.a / big_c;
	const auto b_prim = lab.b / big_c;

	auto big_l = lab.l;
	const auto h = eu::atan2(-lab.b, -lab.a);

	const auto cusp = find_cusp(a_prim, b_prim);
	const auto big_st_max = st_from_cusp(cusp);
	const auto big_s_max = big_st_max.s;
	const auto big_t_max = big_st_max.t;
	const auto big_s_0 = 0.5f;
	const auto k = 1 - big_s_0 / big_s_max;

	// first we find L_v, C_v, L_vt and C_vt

	const auto t = big_t_max / (big_c + big_l * big_t_max);
	const auto big_l_v = t * big_l;
	const auto big_c_v = t * big_c;

	const auto big_l_vt = toe_inv(big_l_v);
	const auto big_c_vt = big_c_v * big_l_vt / big_l_v;

	// we can then use these to invert the step that compensates for the toe and the curved top part of the triangle:
	const auto rgb_scale = linear_from_oklab({.l = big_l_vt, .a = a_prim * big_c_vt, .b = b_prim * big_c_vt});
	const auto scale_big_l = cbrtf(1.f / std::max(std::max(rgb_scale.r, rgb_scale.g), std::max(rgb_scale.b, 0.f)));

	big_l = big_l / scale_big_l;
	// big_c = big_c / scale_big_l;

	// big_c = big_c * toe(big_l) / big_l;
	big_l = toe(big_l);

	// we can now compute v and s:

	const auto v = big_l / big_l_v;
	const auto s = (big_s_0 + big_t_max) * big_c_v / ((big_t_max * big_s_0) + big_t_max * k * big_c_v);

	return {.hue = h, .saturation = s, .value = v};
}


// Returns a smooth approximation of the location of the cusp
// This polynomial was created by an optimization process
// It has been designed so that S_mid < S_max and T_mid < T_max
St get_st_mid(float a_prim, float b_prim)
{
	const auto big_s = 0.11516993f
			+ 1.f
				  / (+7.44778970f + 4.15901240f * b_prim
					 + a_prim
						   * (-2.19557347f + 1.75198401f * b_prim
							  + a_prim
									* (-2.13704948f - 10.02301043f * b_prim
									   + a_prim * (-4.24894561f + 5.38770819f * b_prim + 4.69891013f * a_prim))));

	const auto big_t = 0.11239642f
			+ 1.f
				  / (+1.61320320f - 0.68124379f * b_prim
					 + a_prim
						   * (+0.40370612f + 0.90148123f * b_prim
							  + a_prim
									* (-0.27087943f + 0.61223990f * b_prim
									   + a_prim * (+0.00299215f - 0.45399568f * b_prim - 0.14661872f * a_prim))));

	return {.s = big_s, .t = big_t};
}

struct Cs
{
	float c_0;
	float c_mid;
	float c_max;
};

Cs get_cs(float big_l, float a_prim, float b_prim)
{
	const auto cusp = find_cusp(a_prim, b_prim);

	const auto big_c_max = find_gamut_intersection(a_prim, b_prim, big_l, 1, big_l);
	const auto big_st_max = st_from_cusp(cusp);

	// Scale factor to compensate for the curved part of gamut shape:
	const auto k = big_c_max / std::min((big_l * big_st_max.s), (1 - big_l) * big_st_max.t);

	const auto big_c_mid = [&]
	{
		const auto big_st_mid = get_st_mid(a_prim, b_prim);

		// Use a soft minimum function, instead of a sharp triangle shape to get a smooth value for chroma.
		const auto big_c_a = big_l * big_st_mid.s;
		const auto big_c_b = (1.f - big_l) * big_st_mid.t;
		return 0.9f * k * std::sqrt(std::sqrt(1.f / (1.f / (big_c_a * big_c_a * big_c_a * big_c_a) + 1.f / (big_c_b * big_c_b * big_c_b * big_c_b))));
	}();

	const auto big_c_0 = [&]
	{
		// for C_0, the shape is independent of hue, so ST are constant. Values picked to roughly be the average values of ST.
		const auto big_c_a = big_l * 0.4f;
		const auto big_c_b = (1.f - big_l) * 0.8f;

		// Use a soft minimum function, instead of a sharp triangle shape to get a smooth value for chroma.
		return std::sqrt(1.f / (1.f / (big_c_a * big_c_a) + 1.f / (big_c_b * big_c_b)));
	}();

	return {.c_0 = big_c_0, .c_mid = big_c_mid, .c_max = big_c_max};
}

Rgb srgb_from_okhsl(const OkHsl& hsl)
{
	const auto h = hsl.hue;
	const auto s = hsl.saturation;
	const auto l = hsl.lightness;

	if (l == 1.0f)
	{
		return {1.f, 1.f, 1.f};
	}
	
	if (l == 0.f)
	{
		return {0.f, 0.f, 0.f};
	}

	const auto ap = eu::cos(h);
	const auto bp = eu::sin(h);
	const auto big_l = toe_inv(l);

	const auto cs = get_cs(big_l, ap, bp);
	const auto c_0 = cs.c_0;
	const auto c_mid = cs.c_mid;
	const auto c_max = cs.c_max;

	// Interpolate the three values for C so that:
	// At s=0: dC/ds = C_0, C=0
	// At s=0.8: C=C_mid
	// At s=1.0: C=C_max

	const auto mid = 0.8f;
	const auto mid_inv = 1.25f;

	const auto big_c = [&]() {
		if (s < mid)
		{
			const auto t = mid_inv * s;

			const auto k_1 = mid * c_0;
			const auto k_2 = (1.f - k_1 / c_mid);

			return t * k_1 / (1.f - k_2 * t);
		}
		else
		{
			const auto t = (s - mid) / (1 - mid);

			const auto k_0 = c_mid;
			const auto k_1 = (1.f - mid) * c_mid * c_mid * mid_inv * mid_inv / c_0;
			const auto k_2 = (1.f - (k_1) / (c_max - c_mid));

			return k_0 + t * k_1 / (1.f - k_2 * t);
		}
	}();

	const auto rgb = linear_from_oklab({.l = big_l, .a = big_c * ap, .b = big_c * bp});
	return srgb_from_linear(rgb);
}

OkHsl okhsl_from_srgb(const Rgb& rgb)
{
	const auto lab = oklab_from_linear(linear_from_srgb(rgb));

	const auto big_c = std::sqrt(lab.a * lab.a + lab.b * lab.b);
	const auto a_prim = lab.a / big_c;
	const auto b_prim = lab.b / big_c;

	const auto big_l = lab.l;
	const auto h = eu::atan2(-lab.b, -lab.a);

	const auto cs = get_cs(big_l, a_prim, b_prim);
	const auto big_c_0 = cs.c_0;
	const auto big_c_mid = cs.c_mid;
	const auto big_c_max = cs.c_max;

	// Inverse of the interpolation in okhsl_to_srgb:

	const auto mid = 0.8f;
	const auto mid_inv = 1.25f;

	const float s = [&]() {
	if (big_c < big_c_mid)
	{
		const auto k_1 = mid * big_c_0;
		const auto k_2 = (1.f - k_1 / big_c_mid);

		const auto t = big_c / (k_1 + k_2 * big_c);
		return t * mid;
	}
	else
	{
		const auto k_0 = big_c_mid;
		const auto k_1 = (1.f - mid) * big_c_mid * big_c_mid * mid_inv * mid_inv / big_c_0;
		const auto k_2 = (1.f - (k_1) / (big_c_max - big_c_mid));

		const auto t = (big_c - k_0) / (k_1 + k_2 * (big_c - k_0));
		return mid + (1.f - mid) * t;
	}}();

	const auto l = toe(big_l);
	return {.hue = h, .saturation = s, .lightness = l};
}

Lin_rgb keep_within(Lin_rgb c)
{
	const auto ret = Lin_rgb{.r = keep_within01(c.r), .g = keep_within01(c.g), .b = keep_within01(c.b)};
	return ret;
}

Rgb srgb_from_hsl(const Hsl& hsl)
{
	// https://www.rapidtables.com/convert/color/hsl-to-rgb.html

	const auto h = hsl.hue.as_degrees();
	const auto s = hsl.saturation;
	const auto l = hsl.lightness;

	const auto c = (1.0f - std::fabs(2.0f * l - 1.0f)) * s;
	const auto x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));

	const auto m = l - c / 2.0f;

	const auto ret = [m](float r1, float g1, float b1)
	{
		const auto r = r1 + m;
		const auto g = g1 + m;
		const auto b = b1 + m;

		return Rgb{r, g, b};
	};

	if (h < 60.0f) {
		return ret(c, x, 0);
	}
	if (h < 120.0f) {
		return ret(x, c, 0);
	}
	if (h < 180.0f) {
		return ret(0, c, x);
	}
	if (h < 240.0f) {
		return ret(0, x, c);
	}
	if (h < 300.0f) {
		return ret(x, 0, c);
	}
	return ret(c, 0, x);
}

}
