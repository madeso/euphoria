#pragma once

#include "render/world.h"

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
	CompiledShaderVertexAttributes layout;

	FullScreenGeom();
};

/**
 * @}
*/

}  //  namespace eu::render
