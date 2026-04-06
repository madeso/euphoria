#pragma once

// #include "eu/render/color.h"

namespace eu::render
{
struct CompiledCamera;

/** \addtogroup render Renderer
 *  @{
*/

/// Line + color
struct DebugLine
{
	v3 from;
	v3 to;
	Rgb color;
};

/// Just stores debug render requests and doesn't actually render anything.
/// @see Renderer
struct DebugRender
{
	std::vector<DebugLine> lines;

	void add_line(const v3& from, const v3& to, const Rgb& color);
};

void draw_frustum(DebugRender* debug, const CompiledCamera& camera, const Rgb& color);

/**
 * @}
*/

}  //  namespace eu::render
