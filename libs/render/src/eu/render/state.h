#pragma once

#include "eu/render/constants.h"

#include <optional>

namespace eu::render
{

/** \addtogroup render Renderer
 *  @{
*/



///////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declaration
struct FrameBuffer;
struct TextureCubemap;
struct Texture2d;
struct Uniform;



///////////////////////////////////////////////////////////////////////////////////////////////////
// Enums

enum class Blend
{
	zero,
	one,
	src_color,
	one_minus_src_color,
	dst_color,
	one_minus_dst_color,
	src_alpha,
	one_minus_src_alpha,
	dst_alpha,
	one_minus_dst_alpha,
	constant_color,
	one_minus_constant_color,
	constant_alpha,
	one_minus_constant_alpha,
	src_alpha_saturate,
	src1_color,
	one_minus_src1_color,
	src1_alpha,
	one_minus_src1_alpha
};

enum class Compare
{
	always,
	never,
	less,
	equal,
	less_equal,
	greater,
	not_equal,
	greater_equal
};

enum class CullFace
{
	front,
	back,
	front_and_back
};

enum class RenderMode
{
	fill,
	line,
	point
};

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



///////////////////////////////////////////////////////////////////////////////////////////////////
// Alias

/// Blend mode state consisting of src and dst blend functions
using BlendMode = std::tuple<Blend, Blend>;

/// Stencil function state consisiting of the compare function, the ref and the state.
/// @see \ref StateChanger::stencil_func
using StencilFunc = std::tuple<Compare, i32, u32>;

/// Stencil operation state consisting of the stencil fail, depth fail and the pass action.
using StencilOp = std::tuple<StencilAction, StencilAction, StencilAction>;



///////////////////////////////////////////////////////////////////////////////////////////////////
// State

/// A "cache" for the current open gl state.
struct State
{
	std::optional<bool> cull_face;
	std::optional<CullFace> cull_face_mode;

	std::optional<bool> blending;
	std::optional<BlendMode> blend_mode;

	std::optional<bool> depth_test;
	std::optional<bool> depth_mask;
	std::optional<Compare> depth_func;

	std::optional<bool> stencil_test;
	std::optional<u32> stencil_mask;

	std::optional<RenderMode> render_mode;
	std::optional<StencilFunc> stencil_func;
	std::optional<StencilOp> stencil_op;

	std::optional<int> active_texture;
	std::array<std::optional<unsigned int>, MAX_TEXTURES_SUPPORTED> texture_bound;
};



///////////////////////////////////////////////////////////////////////////////////////////////////
// Changer

/// A helper class to change both the opengl state and the cache.
struct StateChanger
{
	State* states;

	explicit StateChanger(State* s);

	StateChanger& cull_face(bool new_state);
	StateChanger& blending(bool new_state);
	StateChanger& depth_test(bool new_state);
	StateChanger& depth_mask(bool new_state);
	StateChanger& depth_func(Compare new_state);
	StateChanger& stencil_test(bool new_state);

	/// Set a bitmask that is ANDed with the stencil value about to be written to the buffer.
	StateChanger& stencil_mask(u32 new_state);

	/** 
	* @param func the stencil test function that determines whether a fragment passes or is discarded.
	This test function is applied to the stored stencil value and the ref value.
	* @param ref the reference value for the stencil test. The stencil buffer's content is compared to this value.
	* @param mask the mask that is ANDed with both the reference value and the stored stencil value before the test compares them.
	*/
	StateChanger& stencil_func(Compare func, i32 ref, u32 mask);
	StateChanger& render_mode(RenderMode new_state);

	/**
    * @param stencil_fail action to take if the stencil test fails.
    * @param depth_fail action to take if the stencil test passes, but the depth test fails.
    * @param pass action to take if both the stencil and the depth test pass.
	*/
	StateChanger& stencil_op(StencilAction stencil_fail, StencilAction depth_fail, StencilAction pass);
	StateChanger& cull_face_mode(CullFace new_state);
	StateChanger& blend_mode(Blend src, Blend dst);
	StateChanger& activate_texture(int new_texture);
	StateChanger& bind_texture_2d(int slot, unsigned int texture);
	StateChanger& bind_texture_cubemap(int slot, unsigned int texture);
};

void bind_texture_2d(State* states, const Uniform& uniform, const Texture2d& texture);
void bind_texture_2d(State* states, const Uniform& uniform, const FrameBuffer& texture);
void bind_texture_cubemap(State* states, const Uniform& uniform, const TextureCubemap& texture);

/**
 * @}
*/

}  //  namespace eu::render
