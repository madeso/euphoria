#pragma once

#include "eu/render/uniform.h"
#include "eu/render/uniform_buffer.h"
#include "eu/render/vertex_layout.h"

namespace eu::render
{
struct FullScreenGeom;
struct RenderSettings;
struct CompiledGeomVertexAttributes;
struct ShaderProgram;
struct CompiledCamera;
struct ShadowContext;
struct Assets;

/** \addtogroup render Renderer
 *  @{
*/

/// Enum describing on how the model transform is provided.
enum class TransformSource
{
	/// the model source is provided as a mat4 uniform.
	Uniform,

	/// the model source is provided as a (instanced) mat4 attribute
	Instanced_mat4
};

/// "Global state" for the shaders describing the state of the camera.
struct CameraUniformBuffer
{
	UniformBufferSetup setup;

	CompiledUniformProp clip_from_view_uni;
	CompiledUniformProp view_from_world_uni;

	std::unique_ptr<UniformBuffer> buffer;

	void set_props(const CompiledCamera& cc);
};


/// A single color shader.
struct LoadedShader_SingleColor
{
	LoadedShader_SingleColor(
		std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l, const CameraUniformBuffer& desc
	);

	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;
	Uniform tint_color_uni;
	Uniform world_from_local_uni;
};

/// Only writes depth.
/// Useful for rendering shadow maps.
struct LoadedShader_OnlyDepth
{
	LoadedShader_OnlyDepth(
		TransformSource model_source,
		std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l, const CameraUniformBuffer& desc
	);

	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;

	std::optional<Uniform> world_from_local_uni;
};

/// A skybox shader.
struct LoadedShader_Skybox
{
	LoadedShader_Skybox(
		std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l, const CameraUniformBuffer& desc
	);

	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;
	Uniform tex_skybox_uniform;
};

/// Parts of a loaded unlit shader
/// @see \ref LoadedShader_Unlit_Container
struct LoadedShader_Unlit
{
	std::shared_ptr<ShaderProgram> program;

	explicit LoadedShader_Unlit(TransformSource model_source, std::shared_ptr<ShaderProgram> p, const CameraUniformBuffer& desc);

	Uniform tint_color_uni;
	Uniform tex_diffuse_uniform;

	std::optional<Uniform> world_from_local_uni;
};

/// Uniform for a directional light.
/// @see \ref DirectionalLight
struct DirectionalLightUniforms
{
	DirectionalLightUniforms(const ShaderProgram* program, const std::string& base);

	Uniform light_diffuse_color_uni;
	Uniform light_specular_color_uni;
	Uniform dir_uni;
};

/// Uniforms for a point light.
/// @see \ref PointLight
struct PointLightUniforms
{
	PointLightUniforms(const ShaderProgram* program, const std::string& base);

	Uniform light_diffuse_color_uni;
	Uniform light_specular_color_uni;
	Uniform light_attenuation_uni;
	Uniform light_world_uni;
};

/// Uniforms for a frustum light.
/// @see \ref FrustumLight
struct FrustumLightUniforms
{
	FrustumLightUniforms(const ShaderProgram* program, const std::string& base);

	Uniform diffuse_uni;
	Uniform specular_uni;
	Uniform attenuation_uni;
	Uniform clip_from_world_uni;
	Uniform world_pos_uni;
	Uniform tex_cookie_uniform;
};

/// Bitmask for what features each postproc shader wants.
enum class PostProcSetup
{
	none = 0,
	factor = 1 << 1,
	resolution = 1 << 2,
	time = 1 << 3
};
PostProcSetup operator|(PostProcSetup lhs, PostProcSetup rhs);

/// The "base class" for a loaded postproc shader.
struct LoadedPostProcShader
{
	std::shared_ptr<ShaderProgram> program;
	Uniform tex_input_uniform;
	std::optional<Uniform> factor_uni;
	std::optional<Uniform> resolution_uni;
	std::optional<Uniform> time_uni;

	explicit LoadedPostProcShader(std::shared_ptr<ShaderProgram> s, PostProcSetup setup);
};

/// Part of a loaded "default" shader.
/// @see \ref LoadedShader_Default_Container
struct LoadedShader_Default
{
	std::shared_ptr<ShaderProgram> program;

	LoadedShader_Default(
		TransformSource model_source, std::shared_ptr<ShaderProgram> p, const RenderSettings& settings, const CameraUniformBuffer& desc
	);

	Uniform tint_color_uni;
	Uniform tex_directional_light_depth_uni;
	Uniform directional_shadow_clip_from_world_uni;
	Uniform tex_diffuse_uniform;
	Uniform tex_specular_uniform;
	Uniform tex_emissive_uniform;
	Uniform ambient_tint_uni;
	Uniform specular_color_uni;
	Uniform shininess_uni;
	Uniform emissive_factor_uni;

	std::optional<Uniform> world_from_local_uni;

	Uniform view_position_uni;
	Uniform light_ambient_color_uni;

	std::vector<DirectionalLightUniforms> directional_lights;
	std::vector<PointLightUniforms> point_lights;
	std::vector<FrustumLightUniforms> frustum_lights;
};

/// A "named boolean"
enum class UseTransparency
{
	yes,
	no
};

/// Stores information that is useful when selecting a shader part from a shader container.
struct RenderContext
{
	TransformSource model_source;
	UseTransparency use_transparency;
	float gamma; ///< gamma from the rendering settings
	ShadowContext const* shadow_context;

	constexpr RenderContext(TransformSource s, UseTransparency t, float g, const ShadowContext* sc)
		: model_source(s)
		, use_transparency(t)
		, gamma(g)
		, shadow_context(sc)
	{}
};

/// A unlit shader.
struct LoadedShader_Unlit_Container
{
	CompiledGeomVertexAttributes geom_layout;

	LoadedShader_Unlit default_shader;
	LoadedShader_Unlit transparency_shader;

	[[nodiscard]] bool is_loaded() const;
};

/// A default shader
struct LoadedShader_Default_Container
{
	CompiledGeomVertexAttributes geom_layout;

	LoadedShader_Default default_shader;
	LoadedShader_Default transparency_shader;
	LoadedShader_Default default_shader_instance;

	[[nodiscard]] bool is_loaded() const;
};

/// Select the correct sub shader from a container.
[[nodiscard]] const LoadedShader_Unlit& shader_from_container(const LoadedShader_Unlit_Container& container, const RenderContext& rc);

/// Select the correct sub shader from a container.
[[nodiscard]] const LoadedShader_Default& shader_from_container(const LoadedShader_Default_Container& container, const RenderContext& rc);

// todo(Gustav): Rename this to composing shader
/// The shader data for composing a rendered image.
struct RealizeShader
{
	explicit RealizeShader(std::shared_ptr<ShaderProgram> s);

	std::shared_ptr<ShaderProgram> program;
	Uniform tex_input_uniform;
	Uniform tex_blurred_bloom_uniform;

	Uniform use_blur_uniform;
	Uniform gamma_uniform;
	Uniform exposure_uniform;
};

/// Extracts data for a bloom
struct ExtractShader
{
	explicit ExtractShader(std::shared_ptr<LoadedPostProcShader>&& sh);

	std::shared_ptr<LoadedPostProcShader> shader;
	Uniform cutoff_uniform;
	Uniform softness_uniform;
};

struct PingPongBlurShader
{
	explicit PingPongBlurShader(std::shared_ptr<LoadedPostProcShader>&& sh);

	std::shared_ptr<LoadedPostProcShader> shader;
	Uniform is_horizontal_uniform;
};


/// All loaded shaders.
struct ShaderResource
{
	LoadedShader_SingleColor single_color_shader;
	LoadedShader_OnlyDepth depth_transform_uniform;
	LoadedShader_OnlyDepth depth_transform_instanced_mat4;
	LoadedShader_Skybox skybox_shader;

	LoadedShader_Unlit_Container unlit_shader_container;
	LoadedShader_Default_Container default_shader_container;

	/// the realization shader that is always run
	RealizeShader pp_realize;
	ExtractShader pp_extract;
	PingPongBlurShader pp_ping;

	/// verify that the shaders are loaded
	[[nodiscard]] bool is_loaded() const;
};

ShaderResource load_shaders(const Assets& assets, const CameraUniformBuffer& desc, const RenderSettings& settings, const FullScreenGeom& full_screen);

/**
 * @}
*/

}  //  namespace eu::render
