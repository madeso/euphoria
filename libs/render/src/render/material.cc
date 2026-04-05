#include "render/material.h"

#include "assert/assert.h"
#include "base/cint.h"

#include "render/renderer.h"
// #include "render/assets.h"
#include "render/camera.h"
#include "render/render_settings.h"
#include "render/shader.h"
#include "render/shader_resource.h"
#include "render/world.h"
#include "render/state.h"
#include "render/constants.h"


namespace eu::render
{

UnlitMaterial::UnlitMaterial(const ShaderResource& resource)
	: shader_container(&resource.unlit_shader_container)
{
}

void UnlitMaterial::use_shader(const RenderContext& rc)
{
	shader_from_container(*shader_container, rc).program->use();
}

void set_optional_mat(
	ShaderProgram* program, const std::optional<Uniform>& uniform, const std::optional<m4>& transform
)
{
	if (uniform && transform)
	{
		program->set_mat(*uniform, *transform);
	}
	else
	{
		ASSERT(uniform.has_value() == false);
		ASSERT(transform.has_value() == false);
	}
}

v3 vec_from_lin(const Lin_rgb& lin)
{
    return { lin.r, lin.g, lin.b };
}

void UnlitMaterial::set_uniforms(
	const RenderContext& rc, const CompiledCamera&, const std::optional<m4>& world_from_local
)
{
	const auto& shader = shader_from_container(*shader_container, rc);
    shader.program->set_vec4(shader.tint_color_uni, { vec_from_lin(linear_from_srgb(color, rc.gamma)), alpha });
	set_optional_mat(shader.program.get(), shader.world_from_local_uni, world_from_local);
}

void UnlitMaterial::bind_textures(const RenderContext& rc, State* states, Assets* assets)
{
	std::shared_ptr<Texture2d> t = texture;
	if (t == nullptr)
	{
		t = assets->get_white();
	}

	bind_texture_2d(states, shader_from_container(*shader_container, rc).tex_diffuse_uniform, *t);
}

void UnlitMaterial::apply_lights(const RenderContext&, const Lights&, const RenderSettings&, State*, Assets*)
{
	// no lights for unlit material
}

bool UnlitMaterial::is_transparent() const
{
	// todo(Gustav): improve transparency
	return alpha < ALPHA_TRANSPARENCY_LIMIT;
}

DefaultMaterial::DefaultMaterial(const ShaderResource& resource)
	: shader_container(&resource.default_shader_container)
{
}

void DefaultMaterial::use_shader(const RenderContext& rc)
{
	shader_from_container(*shader_container, rc).program->use();
}

void DefaultMaterial::set_uniforms(
	const RenderContext& rc, const CompiledCamera& cc, const std::optional<m4>& world_from_local
)
{
	const auto& shader = shader_from_container(*shader_container, rc);

	shader.program->set_vec4(shader.tint_color_uni, {vec_from_lin(linear_from_srgb(color, rc.gamma)), alpha});
	shader.program->set_vec3(shader.ambient_tint_uni, vec_from_lin(linear_from_srgb(ambient_tint, rc.gamma)));
	shader.program->set_vec3(shader.specular_color_uni, vec_from_lin(linear_from_srgb(specular_color, rc.gamma)));
	shader.program->set_float(shader.shininess_uni, shininess);
	shader.program->set_float(shader.emissive_factor_uni, emissive_factor);

	set_optional_mat(shader.program.get(), shader.world_from_local_uni, world_from_local);
	shader.program->set_vec3(shader.view_position_uni, cc.position);
}

std::shared_ptr<Texture2d> get_or_white(Assets* assets, std::shared_ptr<Texture2d> t)
{
	if (t == nullptr)
	{
		return assets->get_white();
	}
	else
	{
		return t;
	}
};

std::shared_ptr<Texture2d> get_or_black(Assets* assets, std::shared_ptr<Texture2d> t)
{
	if (t == nullptr)
	{
		return assets->get_black();
	}
	else
	{
		return t;
	}
};

void DefaultMaterial::bind_textures(const RenderContext& rc, State* states, Assets* assets)
{
	const auto& shader = shader_from_container(*shader_container, rc);
	bind_texture_2d(states, shader.tex_diffuse_uniform, *get_or_white(assets, diffuse));
	bind_texture_2d(states, shader.tex_specular_uniform, *get_or_white(assets, specular));
	bind_texture_2d(states, shader.tex_emissive_uniform, *get_or_black(assets, emissive));
}

void DefaultMaterial::apply_lights(
	const RenderContext& rc, const Lights& lights, const RenderSettings& settings, State* states, Assets* assets
)
{
	const auto& shader = shader_from_container(*shader_container, rc);
	shader.program->set_vec3(shader.light_ambient_color_uni, vec_from_lin(linear_from_srgb(lights.ambient_color, rc.gamma)) * lights.ambient_strength);

	constexpr auto no_directional_light = ([]() {
		DirectionalLight p;
		p.color = colors::black;
		p.diffuse_strength = 0.0f;
		p.specular_strength = 0.0f;
		return p;
	})();
	constexpr auto no_point_light = ([]() {
		PointLight p;
		p.color = colors::black;
		p.diffuse_strength = 0.0f;
		p.specular_strength = 0.0f;
		return p;
	})();
	auto no_frustum_light = ([]() {
		FrustumLight p;
		p.color = colors::black;
		p.diffuse_strength = 0.0f;
		p.specular_strength = 0.0f;
		return p;
	})();

	// todo(Gustav): graph the most influential lights instead of the first N lights
	for (int index = 0; index < settings.number_of_directional_lights; index += 1)
	{
		const auto& p = sizet_from_int(index) < lights.directional_lights.size()
						  ? lights.directional_lights[sizet_from_int(index)]
						  : no_directional_light;
		const auto& u = shader.directional_lights[sizet_from_int(index)];
		shader.program->set_vec3(u.light_diffuse_color_uni, vec_from_lin(linear_from_srgb(p.color, settings.gamma)) * p.diffuse_strength);
		shader.program->set_vec3(u.light_specular_color_uni, vec_from_lin(linear_from_srgb(p.color, settings.gamma)) * p.specular_strength);
		const auto vectors = create_vectors(p);
		shader.program->set_vec3(u.dir_uni, vectors.front);
	}

	for (int index = 0; index < settings.number_of_point_lights; index += 1)
	{
		const auto& p = sizet_from_int(index) < lights.point_lights.size()
			? lights.point_lights[sizet_from_int(index)]
			: no_point_light
		;
		const auto& u = shader.point_lights[sizet_from_int(index)];
		shader.program->set_vec3(u.light_diffuse_color_uni, vec_from_lin(linear_from_srgb(p.color, settings.gamma)) * p.diffuse_strength);
		shader.program->set_vec3(u.light_specular_color_uni, vec_from_lin(linear_from_srgb(p.color, settings.gamma)) * p.specular_strength);
		shader.program->set_vec3(u.light_world_uni, p.position);
		shader.program->set_vec4(u.light_attenuation_uni, {p.min_range, p.max_range, p.curve.slope, p.curve.threshold});
	}

	for (int index = 0; index < settings.number_of_frustum_lights; index += 1)
	{
		const auto& p = sizet_from_int(index) < lights.frustum_lights.size() ? lights.frustum_lights[sizet_from_int(index)]
																		: no_frustum_light;
		const auto& u = shader.frustum_lights[sizet_from_int(index)];
		shader.program->set_vec3(u.diffuse_uni, vec_from_lin(linear_from_srgb(p.color, settings.gamma)) * p.diffuse_strength);
		shader.program->set_vec3(u.specular_uni, vec_from_lin(linear_from_srgb(p.color, settings.gamma)) * p.specular_strength);
		shader.program->set_vec3(u.world_pos_uni, p.position);
		shader.program->set_vec4(u.attenuation_uni, {p.min_range, p.max_range, p.curve.slope, p.curve.threshold});

		const auto view_from_world = create_view_from_world_mat(p.position, create_vectors(p.rotation));
		const auto clip_from_view = m4::create_perspective(p.fov, p.aspect, 0.1f, p.max_range);
		shader.program->set_mat(u.clip_from_world_uni, clip_from_view * view_from_world);

		bind_texture_2d(states, u.tex_cookie_uniform, *get_or_white(assets, p.cookie));
	}

	// directional light shadows
	ASSERT(rc.shadow_context);
	auto* shadow_map = rc.shadow_context != nullptr ? rc.shadow_context->directional_shadow_map : nullptr;
	if (shadow_map != nullptr)
	{
		bind_texture_2d(states, shader.tex_directional_light_depth_uni, *shadow_map);
	}
	else
	{
		bind_texture_2d(states, shader.tex_directional_light_depth_uni, *assets->get_white());
	}

	const auto directional_shadow_clip_from_world
		= rc.shadow_context != nullptr ? rc.shadow_context->directional_shadow_clip_from_world : m4_identity;
	shader.program->set_mat(shader.directional_shadow_clip_from_world_uni, directional_shadow_clip_from_world);
}

bool DefaultMaterial::is_transparent() const
{
	// todo(Gustav): improve transparency
	return alpha < ALPHA_TRANSPARENCY_LIMIT;
}

}  //  namespace eu::render
