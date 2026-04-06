#pragma once

#include "eu/render/texture.h"

namespace eu::render
{
struct Texture2d;
struct Renderer;
struct Camera;
struct World;
struct RenderWorld;
struct ImguiShaderCache;

/** \addtogroup postproc Post Processing
 * \brief A basic framework for applying post-processing effects and rendering the \ref World.
 *  @{
*/

/// Arguments for rendering a post-processing effect.
struct PostProcArg
{
	const World* world;
	Size window_size;
	const Camera* camera;
	Renderer* renderer;
};

/// A source that can be rendered to a framebuffer or a screen.
/// Pretty much always a "render world" call or the output of an existing \ref RenderTextureWithShader.
struct RenderSource
{
	RenderSource() = default;
	virtual ~RenderSource() = default;

	RenderSource(const RenderSource&) = delete;
	RenderSource(RenderSource&&) = delete;
	void operator=(const RenderSource&) = delete;
	void operator=(RenderSource&&) = delete;

	virtual void render(const PostProcArg& arg) = 0;
};

/// A functor that sends properties/uniforms to the shader.
/// First and foremost this would the used texture/fbo, but it could also be other uniforms like time, resolution, etc.
struct ShaderPropertyProvider
{
	ShaderPropertyProvider() = default;
	virtual ~ShaderPropertyProvider() = default;

	ShaderPropertyProvider(const ShaderPropertyProvider&) = delete;
	ShaderPropertyProvider(ShaderPropertyProvider&&) = delete;
	void operator=(const ShaderPropertyProvider&) = delete;
	void operator=(ShaderPropertyProvider&&) = delete;

	virtual void use_shader(const PostProcArg& a, const FrameBuffer& t) = 0;
};

/// Applies a shader to the output of another source.
/// This is done using a framebuffer that is rendered to a quad with the shader applied.
struct RenderTextureWithShader : RenderSource
{
	std::string name;
	std::shared_ptr<RenderSource> source;
	std::shared_ptr<FrameBuffer> fbo;
	ShaderPropertyProvider* effect;

	RenderTextureWithShader(std::string n, std::shared_ptr<RenderSource> s, std::shared_ptr<FrameBuffer> f, ShaderPropertyProvider* e);

	/// render internal fbo to a quad with a shader
	void render(const PostProcArg& arg) override;

	/// call render on linked source and render to fbo
	void update(const PostProcArg& arg);
};

/// A compiled full-screen-effect.
/// when compiled it could be:
/// * [render world to screen]
/// * [render world to fbo], [render fbo to screen with shader]
/// * [render world to fbo] [[render fbo to intermediate with shader] [render int to screen with shader]]
/// * [render world to fbo] [[render fbo to intermediate with shader] [render int to fbo with shader]] [render fbo to screen with shader]
struct CompiledStack
{
	/// start with a simple world, but depending on the current effect list, could be more...
	std::shared_ptr<RenderSource> last_source;

	std::vector<std::shared_ptr<RenderTextureWithShader>> targets;
};


/// Arguments for when building a effect stack.
/// @see \ref Effect::build
struct BuildArg
{
	CompiledStack* builder;
	Size window_size;
};


struct EffectStack;

/// A effect that can be toggled.
/// Common effects are: blur, bloom, color grading, etc.
/// Most effects from https://lettier.github.io/3d-game-shaders-for-beginners/index.html would be implemented as effects.
struct Effect
{
	Effect() = default;
	virtual ~Effect() = default;

	Effect(const Effect&) = delete;
	Effect(Effect&&) = delete;
	void operator=(const Effect&) = delete;
	void operator=(Effect&&) = delete;

	virtual void build(const BuildArg& arg) = 0;
	virtual void update(float dt) = 0;
	virtual void gui() = 0;

	bool enabled() const;

   protected:

	void set_enabled(bool n);

   private:

	friend struct EffectStack;

	bool is_enabled = false;
	EffectStack* owner = nullptr;
};

/// A special effect where the effect can be eased into existence.
/// Setting it to 0 will disable the effect, setting it anything above will enable it.
struct FactorEffect : Effect
{
	FactorEffect();
	[[nodiscard]] float get_factor() const;
	void set_factor(float f);

   private:

	float factor = 0.0f;
};

/// The facade of the post-proc framework.
struct EffectStack
{
	int current_msaa_setting = -1;
	bool dirty = true;
	std::optional<Size> window_size;
	std::vector<std::shared_ptr<Effect>> effects;
	CompiledStack compiled;

	// render world settings
	// todo(Gustav): is it useful to disable hdr rendering or should that just be removed?
	// todo(Gustav): move exposure to a better place?
	bool use_hdr = true;
	float exposure = 1.0f;

	std::shared_ptr<RenderWorld> render_world;

	/// rebuilds stack if dirty, update all targets, then render the last_source
	void render(const PostProcArg& arg);
	void update(float dt) const;
	void gui(ImguiShaderCache* cache);
};

/**
 * @}
*/

}
