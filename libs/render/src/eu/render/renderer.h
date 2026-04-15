#pragma once

#include "eu/render/debug.h"
#include "eu/render/postproc.h"
#include "eu/render/material.h"
#include "eu/render/render_settings.h"
#include "eu/render/vertex_layout.h"
#include "eu/render/world.h"

namespace eu::render
{

/** \addtogroup render Renderer
 *  @{
*/

struct RendererPimpl;


// todo(Gustav): move to a different file?
struct ShadowContext
{
	FrameBuffer* directional_shadow_map = nullptr;
	m4 directional_shadow_clip_from_world;
};


/// The rendering engine.
struct Renderer
{
    Assets* assets;
	RenderSettings settings;
	DebugRender debug;

	std::unique_ptr<RendererPimpl> pimpl;

	Renderer(State* states, Assets* default_assets, const RenderSettings& settings);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	void operator=(const Renderer&) = delete;
	void operator=(Renderer&&) = delete;

	std::shared_ptr<UnlitMaterial> make_unlit_material() const;
	std::shared_ptr<DefaultMaterial> make_default_material() const;

	[[nodiscard]] CompiledGeomVertexAttributes unlit_geom_layout() const;
	[[nodiscard]] CompiledGeomVertexAttributes default_geom_layout() const;

	std::shared_ptr<FactorEffect> make_invert_effect() const;
	std::shared_ptr<FactorEffect> make_grayscale_effect() const;
	std::shared_ptr<FactorEffect> make_damage_effect() const;
	std::shared_ptr<FactorEffect> make_blur_effect() const;

	[[nodiscard]] Skybox make_skybox(std::shared_ptr<TextureCubemap> texture) const;

	/// verify that the renderer was fully loaded
	[[nodiscard]] bool is_loaded() const;

	/// doesn't set the size, prefer EffectStack::render
	void render_world(const Size& window_size, const World&, const CompiledCamera&, const ShadowContext& shadow_context);

	void render_shadows(const Size& window_size, const World&, const CompiledCamera&) const;
};

/**
 * @}
*/

}  //  namespace eu::render
