#include "log/log.h"


#include "render/shader.source.h"

#include "mustache.hpp"

// #include "default_shader.frag.glsl.h"
// #include "default_shader.vert.glsl.h"
// #include "skybox.frag.glsl.h"
// #include "skybox.vert.glsl.h"

namespace eu::render
{


using Properties = std::unordered_map<std::string, std::string>;

ShaderOptions ShaderOptions::with_transparent_cutoff() const
{
	auto ret = *this;
	ret.transparent_cutoff = true;
	return ret;
}

ShaderOptions ShaderOptions::with_instanced_mat4() const
{
	auto ret = *this;
	ret.use_instancing = true;
	return ret;
}

kainjow::mustache::mustache load_mustache(std::string_view str)
{
	auto input = kainjow::mustache::mustache{std::string{str.begin(), str.end()}};
	if (input.is_valid() == false)
	{
		const auto& error = input.error_message();
		LOG_ERR("Failed to parse mustache: {}", error);
	}

	input.set_custom_escape([](const std::string& s) { return s; });
	return input;
}

std::string generate_blur(std::string_view src, const BlurOptions& options)
{
	auto input = load_mustache(src);
	auto data = kainjow::mustache::data{};

	data["is_horizontal"] = options.blur == BlurType::horizontal;
	data["is_vertical"] = options.blur == BlurType::vertical;
	data["sample_count"] = fmt::format("{}", options.sample_count);
	data["is_gauss"] = options.is_gauss == IsGauss::yes;

	return input.render(data);
}

std::string generate(std::string_view str, const ShaderOptions& options, const std::string& uniform_buffer_source)
{
	auto input = load_mustache(str);
	auto data = kainjow::mustache::data{};

	data["use_lights"] = options.use_lights;
	data["use_blinn_phong"] = options.use_blinn_phong;
	data["use_texture"] = options.use_texture;
	data["number_of_directional_lights"] = fmt::format("{}", options.number_of_directional_lights);
	data["number_of_point_lights"] = fmt::format("{}", options.number_of_point_lights);
	data["number_of_frustum_lights"] = fmt::format("{}", options.number_of_frustum_lights);
	data["transparent_cutoff"] = options.transparent_cutoff;
	data["use_instancing"] = options.use_instancing;
	data["uniform_buffer_source"] = uniform_buffer_source;
	data["only_depth"] = options.only_depth;

	return input.render(data);
}

std::string generate(std::string_view str, const std::string& uniform_buffer_source)
{
	auto input = load_mustache(str);
	auto data = kainjow::mustache::data{};

	data["uniform_buffer_source"] = uniform_buffer_source;

	return input.render(data);
}

ShaderSource_withLayout load_shader_source(const ShaderSource& source, const ShaderOptions& options, const std::string& uniform_buffer_source)
{
	auto layout = ShaderVertexAttributes{{VertexType::position3, "a_position"}, {VertexType::color3, "a_color"}};

	if (options.use_texture)
	{
		layout.emplace_back(VertexElementDescription{VertexType::texture2, "a_tex_coord"});
	}

	if (options.use_lights)
	{
		layout.emplace_back(VertexElementDescription{VertexType::normal3, "a_normal"});
	}

	return ShaderSource_withLayout{
		layout,
		generate(source.vertex, options, uniform_buffer_source),
		generate(source.fragment, options, uniform_buffer_source)
	};
}

ShaderSource load_skybox_source(const ShaderSource& source, const std::string& uniform_buffer_source)
{
	return ShaderSource{
		generate(source.vertex, uniform_buffer_source), generate(source.fragment, uniform_buffer_source)
	};
}

}  //  namespace eu::render
