#include "eu/render/renderer.h"

#include <algorithm>

#include "eu/log/log.h"

#include "eu/render/camera.h"
#include "eu/render/fullscreen.h"
#include "eu/render/geom.builder.h"
#include "eu/render/geom.h"
#include "eu/render/opengl_utils.h"
#include "eu/render/renderer.pimpl.h"
#include "eu/render/state.h"
#include "eu/render/constants.h"

namespace eu::render
{

Renderer::Renderer(Assets* default_assets, const RenderSettings& set)
	: assets(default_assets)
    , settings(set)
	, pimpl(std::make_unique<RendererPimpl>(*assets, set, FullScreenGeom{}))
{
}

Renderer::~Renderer() = default;

Skybox Renderer::make_skybox(std::shared_ptr<TextureCubemap> texture) const
{
	constexpr float size = 1.0f;

	const auto layout = pimpl->shaders_resources.skybox_shader.geom_layout;

	const auto triangle = geom::create_box(size, size, size, geom::NormalsFacing::In, colors::white).to_geom();
	auto geom = compile_geom(USE_DEBUG_LABEL_MANY("skybox") triangle, layout);

	LOG_INFO("Created skybox");
	return {geom, std::move(texture)};
}

struct TransparentMesh
{
	std::shared_ptr<MeshInstance> mesh;
	float squared_distance_to_camera;
};

std::shared_ptr<UnlitMaterial> Renderer::make_unlit_material() const
{
	return std::make_shared<UnlitMaterial>(pimpl->shaders_resources);
}

std::shared_ptr<DefaultMaterial> Renderer::make_default_material() const
{
	return std::make_shared<DefaultMaterial>(pimpl->shaders_resources);
}

CompiledGeomVertexAttributes Renderer::unlit_geom_layout() const
{
	return pimpl->shaders_resources.unlit_shader_container.geom_layout;
}

CompiledGeomVertexAttributes Renderer::default_geom_layout() const
{
	return pimpl->shaders_resources.default_shader_container.geom_layout;
}

bool Renderer::is_loaded() const
{
	return pimpl->shaders_resources.is_loaded() && pimpl->debug_drawer.is_loaded();
}

// todo(Gustav): should functions take shared_ptr?
m4 calc_world_from_local(std::shared_ptr<MeshInstance> m, const CompiledCamera& cc)
{
	const auto calc_fixed_right = [](const n3& normal, const n3& up)
	{
		const auto right = normal.cross_norm(up).value_or(kk::right);
		const auto new_up = right.cross_norm(normal).value_or(kk::up);
		return m4::from_basis(right, new_up, normal);
	};

	const auto calc_fixed_up = [](const n3& normal, const n3& up)
	{
		const auto right = normal.cross_norm(up).value_or(kk::right);
		const auto new_normal = right.cross_norm(up).value_or(kk::up);
		return m4::from_basis(right, up, new_normal);
	};

	const auto translation = m4::from_translation(m->world_position);

	// todo(Gustav): verify that the billboards are oriented correctly, grass in example 3 is twosided...
	switch (m->billboarding)
	{
	case Billboarding::screen:
		{
			const auto rotation = calc_fixed_right(v3::from_to(cc.position, m->world_position).get_normalized().value_or(kk::in), kk::up);
			return translation * rotation;
		}
	case Billboarding::screen_fast:
		{
			// todo(Gustav): move to precalculated or remove?
			const auto rotation = calc_fixed_right(cc.in, kk::up);
			return translation * rotation;
		}
	case Billboarding::axial_y:
		{
			const auto rotation = calc_fixed_up(v3::from_to(cc.position, m->world_position).get_normalized().value_or(kk::in), kk::up);
			return translation * rotation;
		}
	case Billboarding::axial_y_fast:
		{
			// todo(Gustav): move to precalculated or remove?
			const auto rotation = calc_fixed_up(cc.in, kk::up);
			return translation * rotation;
		}
	default:
		{
			const auto rotation = m4::from(Q::from(m->rotation)).value_or(m4_identity);
			return translation * rotation;
		}
	}
};

void batch_lines(LineDrawer* drawer, const std::vector<DebugLine>& debug_lines, float gamma)
{
	for (const auto& line: debug_lines)
	{
		drawer->line(line.from, line.to, linear_from_srgb(line.color, gamma));
	}
	drawer->submit();
}

void render_debug_lines(const std::vector<DebugLine>& debug_lines, State* states, LineDrawer* drawer, const CompiledCamera& compiled_camera, const Size& window_size, float gamma)
{
	if (debug_lines.empty())
	{
		return;
	}

	drawer->shader.use();
	drawer->set_camera(compiled_camera);

	StateChanger{states}.depth_func(Compare::less_equal).depth_test(true);
	drawer->set_line_to_solid();
	batch_lines(drawer, debug_lines, gamma);

	StateChanger{states}.depth_func(Compare::greater).depth_test(true);
	drawer->set_line_to_dash({float_from_int(window_size.width), float_from_int(window_size.height)}, 20.0f, 20.0f);
	batch_lines(drawer, debug_lines, gamma);
}

void Renderer::render_world(const Size& window_size, const World& world, const CompiledCamera& compiled_camera, const ShadowContext& shadow_context)
{
	SCOPED_DEBUG_GROUP("render world call"sv);
	const auto has_outlined_meshes = std::ranges::any_of(world.meshes,
		[](const auto& mesh) { return mesh->outline.has_value(); }
	);
	StateChanger{&pimpl->states}
		.cull_face(true)
		.cull_face_mode(CullFace::back)
		.stencil_mask(0xFF)
		.stencil_test(has_outlined_meshes)
		.depth_test(true)
		.depth_mask(true)
		.stencil_op(StencilAction::keep, StencilAction::replace, StencilAction::replace)
		.blend_mode(Blend::src_alpha, Blend::one_minus_src_alpha);

	const auto clear_color = linear_from_srgb(world.clear_color, settings.gamma);
	glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	auto bound_camera_buffer = BoundUniformBuffer{pimpl->camera_uniform_buffer.buffer.get()};
	pimpl->camera_uniform_buffer.set_props(compiled_camera);

	std::vector<TransparentMesh> transparent_meshes;

	// render solids
	{
		if (world.meshes.empty() == false)
		{
			SCOPED_DEBUG_GROUP("render basic geom"sv);
			for (const auto& mesh: world.meshes)
			{
				const auto not_transparent_context
					= RenderContext{TransformSource::Uniform, UseTransparency::no, settings.gamma, &shadow_context};

				if (mesh->material->is_transparent())
				{
					transparent_meshes.emplace_back(
						TransparentMesh{
						    .mesh = mesh,
                            .squared_distance_to_camera = v3::from_to(compiled_camera.position, mesh->world_position).get_length_squared()
						}
					);

					continue;
				}
				StateChanger{&pimpl->states}
					.depth_test(true)
					.depth_mask(true)
					.depth_func(Compare::less)
					.blending(false)
					.stencil_mask(0x0)
					.stencil_func(Compare::always, 1, 0xFF);

				if (mesh->outline)
				{
					StateChanger{&pimpl->states}.stencil_func(Compare::always, 1, 0xFF).stencil_mask(0xFF);
				}
				mesh->material->use_shader(not_transparent_context);
				mesh->material->set_uniforms(
					not_transparent_context, compiled_camera, calc_world_from_local(mesh, compiled_camera)
				);
				mesh->material->bind_textures(not_transparent_context, &pimpl->states, assets);
				mesh->material->apply_lights(not_transparent_context, world.lights, settings, &pimpl->states, assets);

				render_geom(*mesh->geom);
			}
		}

		if (world.instances.empty() == false)
		{
			SCOPED_DEBUG_GROUP("render instances"sv);
			for (const auto& instance: world.instances)
			{
				const auto not_transparent_context = RenderContext{TransformSource::Instanced_mat4, UseTransparency::no, settings.gamma, &shadow_context};

				StateChanger{&pimpl->states}
					.depth_test(true)
					.depth_mask(true)
					.depth_func(Compare::less)
					.blending(false)
					.stencil_mask(0x0)
					.stencil_func(Compare::always, 1, 0xFF);
				instance->material->use_shader(not_transparent_context);
				instance->material->set_uniforms(
					// todo(Gustav): should we really set the model matrix for instanced meshes?
					not_transparent_context,
					compiled_camera,
					std::nullopt
				);
				instance->material->bind_textures(not_transparent_context, &pimpl->states, assets);
				instance->material->apply_lights(
					not_transparent_context, world.lights, settings, &pimpl->states, assets
				);

				render_geom_instanced(*instance);
			}
		}

		if (debug.lines.empty() == false)
		{
			SCOPED_DEBUG_GROUP("render debug lines"sv);
			render_debug_lines(
				debug.lines, &pimpl->states, &pimpl->debug_drawer, compiled_camera, window_size, settings.gamma
			);
		}
	}

	// todo(Gustav): should this be done at the start of the update so the renderer can be "const"
	debug.lines.clear();

	// render skybox
	if (world.skybox && world.skybox->cubemap != nullptr && world.skybox->geom != nullptr)
	{
		SCOPED_DEBUG_GROUP("render skybox"sv);
		StateChanger{&pimpl->states}
			.depth_test(true)
			.depth_mask(false)
			.depth_func(Compare::less_equal)
			.blending(false)
			.stencil_mask(0x0)
			.stencil_func(Compare::always, 1, 0xFF);

		auto& shader = pimpl->shaders_resources.skybox_shader;

		shader.program->use();
		bind_texture_cubemap(&pimpl->states, shader.tex_skybox_uniform, *world.skybox->cubemap);

		render_geom(*world.skybox->geom);
	}

	std::sort(
		transparent_meshes.begin(),
		transparent_meshes.end(),
		[](const auto& lhs, const auto& rhs) { return lhs.squared_distance_to_camera > rhs.squared_distance_to_camera; }
	);

	if (transparent_meshes.empty() == false)
	{
		SCOPED_DEBUG_GROUP("render transparent meshes"sv);
		for (auto& transparent_mesh: transparent_meshes)
		{
			const auto transparent_context = RenderContext{TransformSource::Uniform, UseTransparency::yes, settings.gamma, &shadow_context};

			const auto& mesh = transparent_mesh.mesh;
			StateChanger{&pimpl->states}
				.depth_test(true)
				.depth_mask(true)
				.depth_func(Compare::less)
				.blending(true)
				.stencil_mask(0x0)
				.stencil_func(Compare::always, 1, 0xFF);

			if (mesh->outline)
			{
				StateChanger{&pimpl->states}.stencil_func(Compare::always, 1, 0xFF).stencil_mask(0xFF);
			}
			mesh->material->use_shader(transparent_context);
			mesh->material->set_uniforms(transparent_context, compiled_camera, calc_world_from_local(mesh, compiled_camera));
			mesh->material->bind_textures(transparent_context, &pimpl->states, assets);
			mesh->material->apply_lights(transparent_context, world.lights, settings, &pimpl->states, assets);

			render_geom(*mesh->geom);
		}
	}

	// render outline over all other meshes
	if (has_outlined_meshes)
	{
		SCOPED_DEBUG_GROUP("render outline meshes"sv);
		for (const auto& mesh: world.meshes)
		{
			if (const auto& mesh_outline = mesh->outline)
			{
				StateChanger{&pimpl->states}
					.stencil_func(Compare::not_equal, 1, 0xFF)
					.stencil_mask(0x00)
					.depth_test(false);
				const m4 small_scale_mat = m4::from_scale(v3{OUTLINE_SCALE, OUTLINE_SCALE, OUTLINE_SCALE});

				auto& shader = pimpl->shaders_resources.single_color_shader;
				shader.program->use();
				shader.program->set_vec4(shader.tint_color_uni, v4{vec_from_lin(linear_from_srgb(*mesh_outline, settings.gamma)), 1});

				shader.program->set_mat(shader.world_from_local_uni, calc_world_from_local(mesh, compiled_camera) * small_scale_mat);

				render_geom(*mesh->geom);
			}
		}
	}
}

void Renderer::render_shadows(const Size& window_size, const World& world, const CompiledCamera& compiled_camera) const
{
	// todo(Gustav): bind less colors and use depth only shaders
	SCOPED_DEBUG_GROUP("render shadows call"sv);
	StateChanger{&pimpl->states}
		.cull_face(true)
		.cull_face_mode(CullFace::back)
		.depth_test(true)
		.depth_mask(true);

	glClear(GL_DEPTH_BUFFER_BIT);

	auto bound_camera_buffer = BoundUniformBuffer{pimpl->camera_uniform_buffer.buffer.get()};
	pimpl->camera_uniform_buffer.set_props(compiled_camera);

	// todo(Gustav): change shader to something that doesn't write color

	StateChanger{&pimpl->states}
		.depth_test(true)
		.depth_mask(true)
		.depth_func(Compare::less)
		.blending(false)
		.stencil_mask(0x0)
		.stencil_func(Compare::always, 1, 0xFF);

	// render solids
	{
		if (world.meshes.empty() == false)
		{
			SCOPED_DEBUG_GROUP("render basic geom"sv);
			for (const auto& mesh: world.meshes)
			{
				if (mesh->material->is_transparent())
				{
					continue;
				}

				auto& shader = pimpl->shaders_resources.depth_transform_uniform;
				shader.program->use();

				// todo(Gustav): the depth shader should not be a shared type between transform uniform and instanced, this should be verified at compile time with different types
				ASSERT(shader.world_from_local_uni.has_value());
				if (shader.world_from_local_uni)
				{
					shader.program->set_mat(*shader.world_from_local_uni, calc_world_from_local(mesh, compiled_camera));
				}

				if (mesh->billboarding != Billboarding::none)
				{
					continue;
				}
				render_geom(*mesh->geom);
			}
		}

		if (world.instances.empty() == false)
		{
			SCOPED_DEBUG_GROUP("render instances"sv);
			for (const auto& instance: world.instances)
			{
				auto& shader = pimpl->shaders_resources.depth_transform_instanced_mat4;
				shader.program->use();
				ASSERT(shader.world_from_local_uni.has_value() == false);

				render_geom_instanced(*instance);
			}
		}

		if (debug.lines.empty() == false)
		{
			SCOPED_DEBUG_GROUP("render debug lines"sv);
			render_debug_lines(
				debug.lines, &pimpl->states, &pimpl->debug_drawer, compiled_camera, window_size, settings.gamma
			);
		}
	}
}

}  //  namespace eu::render
