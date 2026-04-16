#pragma once

#include "eu/render/world.h"

namespace eu::render
{

/** \addtogroup render Renderer
 *  @{
*/

// todo(Gustav): switch to a full screen triangle instead
/// Store the information for how to render a full screen quad.
struct FullScreenGeom
{
	std::shared_ptr<CompiledGeom> geom;
    core::CompiledShaderVertexAttributes layout;

	FullScreenGeom();
};

/**
 * @}
*/

}  //  namespace eu::render
