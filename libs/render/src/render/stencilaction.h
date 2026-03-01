#pragma once

namespace eu::render
{

/** \addtogroup render Renderer
 *  @{
*/


enum class StencilAction
{
	/// The currently stored stencil value is kept.
	keep,
	/// The stencil value is set to 0.
	zero,
	/// The stencil value is replaced with the reference value set with stencil_func.
	replace,
	/// The stencil value is increased by 1 if it is lower than the maximum value.
	increase,
	/// Same as increase, but wraps it back to 0 as soon as the maximum value is exceeded.
	increase_wrap,
	/// The stencil value is decreased by 1 if it is higher than the minimum value.
	decrease,
	/// Same as decrease, but wraps it to the maximum value if it ends up lower than 0.
	decrease_wrap,
	/// Bitwise inverts the current stencil buffer value.
	invert
};

/**
 * @}
*/

}  //  namespace klotter
