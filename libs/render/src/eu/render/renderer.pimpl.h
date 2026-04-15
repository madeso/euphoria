#pragma once

#include "eu/render/linebatch.h"
#include "eu/render/state.h"
#include "eu/render/shader_resource.h"
#include "eu/render/world.h"

namespace eu::render
{

/** \addtogroup render Renderer
 *  @{
*/

/// Internal state of the renderer.
struct RendererPimpl
{
    State* states;
	CameraUniformBuffer camera_uniform_buffer;
	ShaderResource shaders_resources;
	LineDrawer debug_drawer;
	std::shared_ptr<CompiledGeom> full_screen_geom;

	RendererPimpl(State* states, const Assets& assets, const RenderSettings& set, const FullScreenGeom& full_screen);
};

/**
 * @}
*/

}  //  namespace eu::render
