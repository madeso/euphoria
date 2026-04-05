#pragma once

namespace eu::render
{

/** \addtogroup render Renderer
 *  @{
*/

// todo(Gustav): figure out what values should force the renderer to restart.
/// Startup settings for the renderer.
struct RenderSettings
{
	int number_of_directional_lights = 5;
	int number_of_point_lights = 5;
	int number_of_frustum_lights = 5;

	/// Should bloom be used?
	/// The renderer doesn't need to restart when this value has changed.
	/// The effect stack needs to be rebuilt, but that should happen automatically.
	bool use_bloom = true;

	/// Number of msaa samples to use.
	/// The renderer doesn't need to restart when this value has changed.
	/// The effect stack needs to be rebuilt, but that should happen automatically.
	int msaa = 4;

	/// The gamma correction value.
	/// The renderer doesn't need to restart when this value has changed.
	float gamma = 2.2f;

	/// The bloom cutoff value.
	/// If brightness goes over this value, it will contribute to the bloom.
	/// The renderer doesn't need to restart when this value has changed.
	float bloom_cutoff = 1.0f;

	/// The bloom softness.
	/// Also called knee, 0=no softness, 1=max softness.
	/// The renderer doesn't need to restart when this value has changed.
	float bloom_softness = 0.25f;

	/// how any steps of bloom blur to perform.
	/// The renderer doesn't need to restart when this value has changed.
	int bloom_blur_steps = 10;

	/// The resolution of the shadow map.
	/// The renderer needs to restart when this value has changed.
	Size shadow_map_resolution = {2048, 2048};

	/// Use a tight fit shadow map.
	/// The renderer doesn't need to restart when this value has changed.
	bool use_tight_fit_shadows = false;
};

/**
 * @}
*/

}  //  namespace eu::render
