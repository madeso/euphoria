#pragma once

#include "render/blend.h"
#include "render/compare.h"
#include "render/cullface.h"
#include "render/rendermode.h"
#include "render/stencilaction.h"

namespace eu::render
{
struct FrameBuffer;
struct TextureCubemap;
struct Texture2d;
struct Uniform;
struct OpenglStates;

/** \addtogroup render Renderer
 *  @{
*/

/// A helper class to change both the opengl state and the cache.
struct StateChanger
{
	OpenglStates* states;

	explicit StateChanger(OpenglStates* s);

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

void bind_texture_2d(OpenglStates* states, const Uniform& uniform, const Texture2d& texture);
void bind_texture_2d(OpenglStates* states, const Uniform& uniform, const FrameBuffer& texture);
void bind_texture_cubemap(OpenglStates* states, const Uniform& uniform, const TextureCubemap& texture);

/**
 * @}
*/

}  //  namespace klotter
