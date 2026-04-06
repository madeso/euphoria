#pragma once

#include "eu/render/vertex_layout.h"

namespace eu::render
{

// todo(Gustav): Should this be merged with the RenderSettings
/// Shader compilation options.
/// @see \ref RenderSettings
struct ShaderOptions
{
	bool only_depth = false;
	bool use_blinn_phong = true;
	bool use_texture = false;
	bool use_lights = false;

	int number_of_point_lights = 0;
	int number_of_frustum_lights = 0;
	int number_of_directional_lights = 0;

	bool transparent_cutoff = false;
	[[nodiscard]] ShaderOptions with_transparent_cutoff() const;

	bool use_instancing = false;
	[[nodiscard]] ShaderOptions with_instanced_mat4() const;
};

/// Shader source with the layout that is expected.
/// @see \ref ShaderSource
struct ShaderSource_withLayout
{
	ShaderVertexAttributes layout;
	std::string vertex;
	std::string fragment;
};

// vertex and fragment shader source.
/// @see \ref ShaderSource_withLayout
struct ShaderSource
{
	std::string vertex;
	std::string fragment;
};

/// @see \ref generate_blur
enum class BlurType
{
	horizontal,
	vertical
};

/// @see \ref generate_blur
enum class IsGauss
{
	no,
	yes
};

/// Options for generating a blur shader source.
/// @see \ref generate_blur
struct BlurOptions
{
	BlurType blur;
	int sample_count;
	IsGauss is_gauss;
};

std::string generate_blur(std::string_view src, const BlurOptions& options);

ShaderSource_withLayout load_shader_source(const ShaderSource& source, const ShaderOptions& options, const std::string& uniform_buffer_source);

ShaderSource load_skybox_source(const ShaderSource& source, const std::string& uniform_buffer_source);

}  //  namespace eu::render
