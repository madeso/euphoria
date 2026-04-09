#include "eu/render/shader_resource.h"

#include "eu/assert/assert.h"



#include "eu/render/camera.h"
#include "eu/render/constants.h"
#include "eu/render/fullscreen.h"
#include "eu/render/render_settings.h"
#include "eu/render/shader.h"
#include "eu/render/shader.source.h"


namespace eu::render
{



void CameraUniformBuffer::set_props(const CompiledCamera& cc) // NOLINT(readability-make-member-function-const)
{
	buffer->set_mat4(clip_from_view_uni, cc.clip_from_view);
	buffer->set_mat4(view_from_world_uni, cc.view_from_world);
}



LoadedShader_SingleColor::LoadedShader_SingleColor(
	std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l, const CameraUniformBuffer& desc
)
	: program(std::move(p))
	, geom_layout(std::move(l))
	, tint_color_uni(program->get_uniform("u_material.diffuse_tint"))
	, world_from_local_uni(program->get_uniform("u_world_from_local"))
{
	program->setup_uniform_block(desc.setup);
}



LoadedShader_Skybox::LoadedShader_Skybox(
	std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l, const CameraUniformBuffer& desc
)
	: program(std::move(p))
	, geom_layout(std::move(l))
	, tex_skybox_uniform(program->get_uniform("u_skybox_tex"))
{
	setup_textures(program.get(), {&tex_skybox_uniform});
	program->setup_uniform_block(desc.setup);
}

LoadedShader_OnlyDepth::LoadedShader_OnlyDepth(
	TransformSource model_source,
	std::shared_ptr<ShaderProgram> p,
	CompiledGeomVertexAttributes l,
	const CameraUniformBuffer& desc
)
	: program(std::move(p))
	, geom_layout(std::move(l))
	, world_from_local_uni(
		  model_source == TransformSource::Uniform ? std::optional<Uniform>{program->get_uniform("u_world_from_local")}
												   : std::nullopt
	  )
{
	program->setup_uniform_block(desc.setup);
}

LoadedShader_Unlit::LoadedShader_Unlit(
	TransformSource model_source,
	std::shared_ptr<ShaderProgram> p,
	const CameraUniformBuffer& desc
)
	: program(std::move(p))
	, tint_color_uni(program->get_uniform("u_material.diffuse_tint"))
	, tex_diffuse_uniform(program->get_uniform("u_material.diffuse_tex"))
	, world_from_local_uni(
		  model_source == TransformSource::Uniform ? std::optional<Uniform>{program->get_uniform("u_world_from_local")} : std::nullopt
	  )
{
	setup_textures(program.get(), {&tex_diffuse_uniform});
	program->setup_uniform_block(desc.setup);
}



DirectionalLightUniforms::DirectionalLightUniforms(const ShaderProgram* program, const std::string& base)
	: light_diffuse_color_uni(program->get_uniform(base + "diffuse"))
	, light_specular_color_uni(program->get_uniform(base + "specular"))
	, dir_uni(program->get_uniform(base + "dir"))
{
}



PointLightUniforms::PointLightUniforms(const ShaderProgram* program, const std::string& base)
	: light_diffuse_color_uni(program->get_uniform(base + "diffuse"))
	, light_specular_color_uni(program->get_uniform(base + "specular"))
	, light_attenuation_uni(program->get_uniform(base + "attenuation"))
	, light_world_uni(program->get_uniform(base + "world_pos"))
{
}



FrustumLightUniforms::FrustumLightUniforms(const ShaderProgram* program, const std::string& base)
	: diffuse_uni(program->get_uniform(base + "diffuse"))
	, specular_uni(program->get_uniform(base + "specular"))
	, attenuation_uni(program->get_uniform(base + "attenuation"))
	, clip_from_world_uni(program->get_uniform(base + "clip_from_world"))
	, world_pos_uni(program->get_uniform(base + "world_pos"))
	, tex_cookie_uniform(program->get_uniform(base + "cookie"))
{
}



PostProcSetup operator|(PostProcSetup lhs, PostProcSetup rhs)
{
	return static_cast<PostProcSetup>(base_cast(lhs) | base_cast(rhs));
}



std::optional<Uniform> get_uniform(
	ShaderProgram& prog, const std::string& name, PostProcSetup setup, PostProcSetup flag
)
{
	if (is_flag_set(setup, flag))
	{
		return prog.get_uniform(name);
	}
	else
	{
		return std::nullopt;
	}
}



LoadedPostProcShader::LoadedPostProcShader(std::shared_ptr<ShaderProgram> s, PostProcSetup setup)
	: program(std::move(s))
	, tex_input_uniform(program->get_uniform("u_texture"))
	, factor_uni(get_uniform(*program, "u_factor", setup, PostProcSetup::factor))
	, resolution_uni(get_uniform(*program, "u_resolution", setup, PostProcSetup::resolution))
	, time_uni(get_uniform(*program, "u_time", setup, PostProcSetup::time))
{
	setup_textures(program.get(), {&tex_input_uniform});
}



LoadedShader_Default::LoadedShader_Default(
	TransformSource model_source,
	std::shared_ptr<ShaderProgram> p,
	const RenderSettings& settings,
	const CameraUniformBuffer& desc
)
	: program(std::move(p))
	, tint_color_uni(program->get_uniform("u_material.diffuse_tint"))
	, tex_directional_light_depth_uni(program->get_uniform("u_directional_light_depth_tex"))
	, directional_shadow_clip_from_world_uni(program->get_uniform("u_directional_shadow_clip_from_world"))
	, tex_diffuse_uniform(program->get_uniform("u_material.diffuse_tex"))
	, tex_specular_uniform(program->get_uniform("u_material.specular_tex"))
	, tex_emissive_uniform(program->get_uniform("u_material.emissive_tex"))
	, ambient_tint_uni(program->get_uniform("u_material.ambient_tint"))
	, specular_color_uni(program->get_uniform("u_material.specular_tint"))
	, shininess_uni(program->get_uniform("u_material.shininess"))
	, emissive_factor_uni(program->get_uniform("u_material.emissive_factor"))
	, world_from_local_uni(
		  model_source == TransformSource::Uniform ? std::optional<Uniform>{program->get_uniform("u_world_from_local")} : std::nullopt
	  )
	, view_position_uni(program->get_uniform("u_view_position"))
	, light_ambient_color_uni(program->get_uniform("u_ambient_light"))
{
	for (int index = 0; index < settings.number_of_directional_lights; index += 1)
	{
		const std::string base = fmt::format("u_directional_lights[{}].", index);
		directional_lights.emplace_back(program.get(), base);
	}

	for (int index = 0; index < settings.number_of_point_lights; index += 1)
	{
		const std::string base = fmt::format("u_point_lights[{}].", index);
		point_lights.emplace_back(program.get(), base);
	}

	for (int index = 0; index < settings.number_of_frustum_lights; index += 1)
	{
		const std::string base = fmt::format("u_frustum_lights[{}].", index);
		frustum_lights.emplace_back(program.get(), base);
	}

	std::vector<Uniform*> textures = {&tex_directional_light_depth_uni, &tex_diffuse_uniform, &tex_specular_uniform, &tex_emissive_uniform};
	for (auto& fl: frustum_lights)
	{
		textures.emplace_back(&fl.tex_cookie_uniform);
	}

	setup_textures(program.get(), textures);
	program->setup_uniform_block(desc.setup);
}



bool LoadedShader_Unlit_Container::is_loaded() const
{
	return default_shader.program->is_loaded() && transparency_shader.program->is_loaded();
}



const LoadedShader_Unlit& shader_from_container(const LoadedShader_Unlit_Container& container, const RenderContext& rc)
{
	return rc.use_transparency == UseTransparency::yes ? container.transparency_shader : container.default_shader;
}

const LoadedShader_Default& shader_from_container(const LoadedShader_Default_Container& container, const RenderContext& rc)
{
	switch (rc.model_source)
	{
	case TransformSource::Uniform:
		return rc.use_transparency == UseTransparency::yes ? container.transparency_shader : container.default_shader;
	case TransformSource::Instanced_mat4:
		ASSERT(rc.use_transparency == UseTransparency::no);	 // not currently supporting instanced transparency
		return container.default_shader_instance;
	default: ASSERT(false && "unhandled"); return container.default_shader;
	}
}




bool LoadedShader_Default_Container::is_loaded() const
{
	return default_shader.program->is_loaded() && transparency_shader.program->is_loaded();
}




RealizeShader::RealizeShader(std::shared_ptr<ShaderProgram> s)
	: program(std::move(s))
	, tex_input_uniform(program->get_uniform("u_texture"))
	, tex_blurred_bloom_uniform(program->get_uniform("u_blurred_bloom"))
	, use_blur_uniform(program->get_uniform("u_use_blur"))
	, gamma_uniform(program->get_uniform("u_gamma"))
	, exposure_uniform(program->get_uniform("u_exposure"))
{
	setup_textures(program.get(), {&tex_input_uniform, &tex_blurred_bloom_uniform});
}



ExtractShader::ExtractShader(std::shared_ptr<LoadedPostProcShader>&& sh)
	: shader(std::move(sh))
	, cutoff_uniform(shader->program->get_uniform("u_cutoff"))
	, softness_uniform(shader->program->get_uniform("u_softness"))
{
}

PingPongBlurShader::PingPongBlurShader(std::shared_ptr<LoadedPostProcShader>&& sh)
	: shader(std::move(sh))
	, is_horizontal_uniform(shader->program->get_uniform("u_is_horizontal"))
{
}



bool ShaderResource::is_loaded() const
{
	return single_color_shader.program->is_loaded()
		&& depth_transform_uniform.program->is_loaded()
		&& depth_transform_instanced_mat4.program->is_loaded()
		&& skybox_shader.program->is_loaded()
		&& unlit_shader_container.is_loaded()
		&& default_shader_container.is_loaded()
		&& pp_realize.program->is_loaded();
}



using BaseShaderData = std::vector<VertexType>;



template<std::size_t count>
BaseShaderData get_vertex_types(const std::array<const ShaderVertexAttributes*, count>& vas)
{
	std::set<VertexType> unique_types;
	auto ret = BaseShaderData{};
	for (const auto& va: vas)
	{
		for (const auto& v: *va)
		{
			const auto [_, is_unique] = unique_types.insert(v.type);
			if (is_unique)
			{
				ret.emplace_back(v.type);
			}
		}
	}
	return ret;
}



// todo(Gustav): should this be a tuple instead? this way the members are named
struct LoadedShader
{
	LoadedShader(std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l)
		: program(std::move(p))
		, geom_layout(std::move(l))
	{}

	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;
};

std::optional<int> get_instance_start_index(const LoadedShader* shader)
{
	if (shader == nullptr)
	{
		return std::nullopt;
	}

	const CompiledGeomVertexAttributes& layout = shader->geom_layout;

	int max = 0;

	for (const auto& entry: layout.elements)
	{
		max = std::max(entry.index, max);
	}

	return max + 1;
}

LoadedShader load_shader(DEBUG_LABEL_ARG_MANY const BaseShaderData& base_layout, const ShaderSource_withLayout& source, TransformSource model_source, const LoadedShader* instance_base = nullptr)
{
	auto layout_compiler = compile_attribute_layouts(base_layout, {source.layout});
	const auto geom_layout = get_geom_layout(layout_compiler);

	std::optional<InstanceProp> instance_prop = std::nullopt;
	std::optional<int> start_index = std::nullopt;
	switch (model_source)
	{
	case TransformSource::Instanced_mat4:
		instance_prop = InstanceProp{VertexType::instance_transform, "u_world_from_local"};
		start_index = get_instance_start_index(instance_base);
		break;
	case TransformSource::Uniform: break;
	default: ASSERT(false && "unhandled ModelSource");
	}

	const auto compiled_layout = compile_shader_layout(layout_compiler, source.layout, instance_prop, start_index);

	auto program = std::make_shared<ShaderProgram>(USE_DEBUG_LABEL_MANY(debug_label) source.vertex, source.fragment, compiled_layout);

	return {program, geom_layout};
}



ShaderResource load_shaders(const Assets& assets, const CameraUniformBuffer& desc, const RenderSettings& settings, const FullScreenGeom& full_screen)
{
    const auto& default_shader_source = assets.default_shader_source;
    const auto& skybox_shader_source = assets.skybox_shader_source;

	const auto single_color_shader = load_shader_source(default_shader_source, {}, desc.setup.source);

	ShaderOptions depth_shader_options;
	depth_shader_options.only_depth = true;
	const auto depth_transform_uniform = load_shader_source(default_shader_source, depth_shader_options, desc.setup.source);
	const auto depth_transform_instanced_mat4 = load_shader_source(default_shader_source, depth_shader_options.with_instanced_mat4(), desc.setup.source);

	const auto skybox_source = load_skybox_source(skybox_shader_source, desc.setup.source);
	const auto skybox_shader = ShaderSource_withLayout{
		ShaderVertexAttributes{{VertexType::position3, "a_position"}}, skybox_source.vertex, skybox_source.fragment
	};

	const BaseShaderData global_shader_data = get_vertex_types<3>
		({
			&single_color_shader.layout,
			&depth_transform_uniform.layout, 
			&depth_transform_instanced_mat4.layout
		});

	ShaderOptions unlit_shader_options;
	unlit_shader_options.use_texture = true;

	ShaderOptions default_shader_options;
	default_shader_options.use_lights = true;
	default_shader_options.use_texture = true;
	default_shader_options.number_of_directional_lights = settings.number_of_directional_lights;
	default_shader_options.number_of_point_lights = settings.number_of_point_lights;
	default_shader_options.number_of_frustum_lights = settings.number_of_frustum_lights;

	auto loaded_unlit = load_shader(
		USE_DEBUG_LABEL_MANY("unlit")
		global_shader_data,
		load_shader_source(default_shader_source, unlit_shader_options.with_transparent_cutoff(), desc.setup.source),
		TransformSource::Uniform
	);
	auto loaded_default = load_shader(
		USE_DEBUG_LABEL_MANY("default")
		global_shader_data,
		load_shader_source(default_shader_source, default_shader_options.with_transparent_cutoff(), desc.setup.source),
		TransformSource::Uniform
	);
	auto loaded_default_instanced = load_shader(
		USE_DEBUG_LABEL_MANY("default instanced")
		global_shader_data,
		load_shader_source(default_shader_source, default_shader_options.with_transparent_cutoff().with_instanced_mat4(), desc.setup.source),
		TransformSource::Instanced_mat4
	);

	auto loaded_unlit_transparency = load_shader(
		USE_DEBUG_LABEL_MANY("unlit transparency")
		global_shader_data, load_shader_source(default_shader_source, unlit_shader_options, desc.setup.source), TransformSource::Uniform
	);
	auto loaded_default_transparency = load_shader(
		USE_DEBUG_LABEL_MANY("default transparency")
		global_shader_data, load_shader_source(default_shader_source, default_shader_options, desc.setup.source), TransformSource::Uniform
	);

	// todo(Gustav): should the asserts here be runtime errors? currently all setups are compile-time...
	ASSERT(loaded_unlit.geom_layout.debug_types == loaded_unlit_transparency.geom_layout.debug_types);
	ASSERT(loaded_default.geom_layout.debug_types == loaded_default_transparency.geom_layout.debug_types);
	ASSERT(
		loaded_default.geom_layout.debug_types == loaded_default_instanced.geom_layout.debug_types
	);	// is this valid? should this fail?

#if 0

	auto pp_invert = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_DEBUG_LABEL_MANY("pp invert")
			std::string{PP_VERT_GLSL}, std::string{PP_INVERT_FRAG_GLSL}, full_screen.layout
		),
		PostProcSetup::factor
	);
	auto pp_grayscale = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_DEBUG_LABEL_MANY("pp grayscale")
			std::string{PP_VERT_GLSL}, std::string{PP_GRAYSCALE_FRAG_GLSL}, full_screen.layout
		),
		PostProcSetup::factor
	);
	auto pp_damage = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_DEBUG_LABEL_MANY("pp damage")
			std::string{PP_VERT_GLSL}, std::string{PP_DAMAGE_FRAG_GLSL}, full_screen.layout
		),
		PostProcSetup::factor | PostProcSetup::resolution | PostProcSetup::time
	);

	constexpr IsGauss use_gauss =
#if FF_HAS(BLUR_USE_GAUSS)
		IsGauss::yes
#else
		IsGauss::no
#endif
		;

	auto pp_blurv = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_DEBUG_LABEL_MANY("pp blur vert")
			std::string{PP_VERT_GLSL},
			generate_blur(PP_BLUR_FRAG_GLSL, {BlurType::vertical, BLUR_SAMPLES, use_gauss}),
			full_screen.layout
		),
		PostProcSetup::factor
	);
	auto pp_blurh = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_DEBUG_LABEL_MANY("pp blur hor")
			std::string{PP_VERT_GLSL},
			generate_blur(PP_BLUR_FRAG_GLSL, {BlurType::horizontal, BLUR_SAMPLES, use_gauss}),
			full_screen.layout
		),
		PostProcSetup::factor | PostProcSetup::resolution
	);
#endif

    const auto& PP_VERT_GLSL = assets.pp_vert_glsl;
    const auto& PP_REALIZE_FRAG_GLSL = assets.pp_realize_frag_glsl;
    const auto& PP_EXTRACT_FRAG_GLSL = assets.pp_extract_frag_glsl;
    const auto& PP_PING_PONG_BLUR_FRAG_GLSL = assets.pp_ping_pong_blur_frag_glsl;

	auto pp_realize = RealizeShader{std::make_shared<ShaderProgram>(
			USE_DEBUG_LABEL_MANY("pp realize")
			std::string{PP_VERT_GLSL}, std::string{PP_REALIZE_FRAG_GLSL}, full_screen.layout
		)};
	auto pp_extract = ExtractShader{std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_DEBUG_LABEL_MANY("pp extract") std::string{PP_VERT_GLSL},
			std::string{PP_EXTRACT_FRAG_GLSL},
			full_screen.layout
		),
		PostProcSetup::none
	)};
	auto pp_ping = PingPongBlurShader{std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_DEBUG_LABEL_MANY("pp ping-pong") std::string{PP_VERT_GLSL},
			std::string{PP_PING_PONG_BLUR_FRAG_GLSL},
			full_screen.layout
		),
		PostProcSetup::none
	)};

	auto loaded_single_color = load_shader(
		USE_DEBUG_LABEL_MANY("single color") global_shader_data, single_color_shader, TransformSource::Uniform
	);
	auto loaded_depth_transform_uniform = load_shader(
		USE_DEBUG_LABEL_MANY("depth transform uniform") global_shader_data, depth_transform_uniform, TransformSource::Uniform
	);
	auto loaded_depth_transform_instanced_mat4 = load_shader(
		USE_DEBUG_LABEL_MANY("depth transform instanced") global_shader_data, depth_transform_instanced_mat4, TransformSource::Instanced_mat4, &loaded_default_instanced
	);
	auto loaded_skybox_shader
		= load_shader(USE_DEBUG_LABEL_MANY("skybox"){}, skybox_shader, TransformSource::Uniform);


	return ShaderResource{
		// todo(Gustav): not really happy with sending "the same" argument twice, loaded_X.program and loaded_X.geom_layout
		.single_color_shader = LoadedShader_SingleColor{std::move(loaded_single_color.program), loaded_single_color.geom_layout, desc},
		.depth_transform_uniform = LoadedShader_OnlyDepth{
			TransformSource::Uniform, std::move(loaded_depth_transform_uniform.program),
			loaded_depth_transform_uniform.geom_layout,
			desc
		},
		.depth_transform_instanced_mat4 = LoadedShader_OnlyDepth{
			TransformSource::Instanced_mat4,
			std::move(loaded_depth_transform_instanced_mat4.program),
			loaded_depth_transform_instanced_mat4.geom_layout,
			desc
		},
		.skybox_shader = LoadedShader_Skybox{std::move(loaded_skybox_shader.program), loaded_skybox_shader.geom_layout, desc},
		.unlit_shader_container = LoadedShader_Unlit_Container{
			loaded_unlit.geom_layout,
			LoadedShader_Unlit{TransformSource::Uniform, std::move(loaded_unlit.program), desc},
			LoadedShader_Unlit{TransformSource::Uniform, std::move(loaded_unlit_transparency.program), desc}
		},
		.default_shader_container = LoadedShader_Default_Container{
			loaded_default.geom_layout,
			LoadedShader_Default{TransformSource::Uniform, std::move(loaded_default.program), settings, desc},
			LoadedShader_Default{TransformSource::Uniform, std::move(loaded_default_transparency.program), settings, desc},
			LoadedShader_Default{TransformSource::Instanced_mat4, std::move(loaded_default_instanced.program), settings, desc}
		},
		// .pp_invert = pp_invert,
		// .pp_grayscale = pp_grayscale,
		// .pp_damage = pp_damage,
		// .pp_blurv = pp_blurv,
		// .pp_blurh = pp_blurh,
		.pp_realize = pp_realize,
		.pp_extract = pp_extract,
		.pp_ping = pp_ping
	};
}



}  //  namespace eu::render
