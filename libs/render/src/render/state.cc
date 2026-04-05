#include "render/state.h"


#include "assert/assert.h"
#include "base/cint.h"

#include "render/texture.h"
#include "render/uniform.h"
#include "render/opengl_utils.h"

namespace eu::render
{

template<typename T>
bool should_change(std::optional<T>* current_state, T new_state)
{
	// if there is a value, and that is the same... then don't update opengl
	if (*current_state && *current_state == new_state)
	{
		return false;
	}

	*current_state = new_state;
	return true;
}

void apply(std::optional<bool>* current_state, bool new_state, GLenum gl_type)
{
	if (should_change(current_state, new_state))
	{
		if (new_state)
		{
			glEnable(gl_type);
		}
		else
		{
			glDisable(gl_type);
		}
	}
}

GLenum enum_from_c(Compare new_state)
{
	switch (new_state)
	{
	case Compare::always: return GL_ALWAYS;
	case Compare::never: return GL_NEVER;
	case Compare::less: return GL_LESS;
	case Compare::equal: return GL_EQUAL;
	case Compare::less_equal: return GL_LEQUAL;
	case Compare::greater: return GL_GREATER;
	case Compare::not_equal: return GL_NOTEQUAL;
	case Compare::greater_equal: return GL_GEQUAL;
	default: DIE("Invalid depth func"); return GL_LESS;
	}
}

GLenum enum_from_sa (StencilAction sa)
{
	switch (sa)
	{
	case StencilAction::keep: return GL_KEEP;
	case StencilAction::zero: return GL_ZERO;
	case StencilAction::replace: return GL_REPLACE;
	case StencilAction::increase: return GL_INCR;
	case StencilAction::increase_wrap: return GL_INCR_WRAP;
	case StencilAction::decrease: return GL_DECR;
	case StencilAction::decrease_wrap: return GL_DECR_WRAP;
	case StencilAction::invert: return GL_INVERT;
	default: DIE("Invalid stencil action"); return GL_KEEP;
	}
};

StateChanger::StateChanger(State* s)
	: states(s)
{
}

StateChanger& StateChanger::cull_face(bool new_state)
{
	apply(&states->cull_face, new_state, GL_CULL_FACE);
	return *this;
}

StateChanger& StateChanger::blending(bool new_state)
{
	apply(&states->blending, new_state, GL_BLEND);
	return *this;
}

StateChanger& StateChanger::depth_test(bool new_state)
{
	apply(&states->depth_test, new_state, GL_DEPTH_TEST);
	return *this;
}

StateChanger& StateChanger::depth_mask(bool new_state)
{
	if (should_change(&states->depth_mask, new_state))
	{
		glDepthMask(new_state ? GL_TRUE : GL_FALSE);
	}
	return *this;
}

StateChanger& StateChanger::depth_func(Compare new_state)
{
	if (should_change(&states->depth_func, new_state))
	{
		const auto mode = enum_from_c(new_state);
		glDepthFunc(mode);
	}

	return *this;
}

StateChanger& StateChanger::stencil_test(bool new_state)
{
	apply(&states->stencil_test, new_state, GL_STENCIL_TEST);
	return *this;
}

/// Set a bitmask that is ANDed with the stencil value about to be written to the buffer.
StateChanger& StateChanger::stencil_mask(u32 new_state)
{
	if (should_change(&states->stencil_mask, new_state))
	{
		glStencilMask(new_state);
	}

	return *this;
}

StateChanger& StateChanger::stencil_func(Compare func, i32 ref, u32 mask)
{
	if (should_change(&states->stencil_func, {func, ref, mask}))
	{
		glStencilFunc(enum_from_c(func), ref, mask);
	}

	return *this;
}

StateChanger& StateChanger::render_mode(RenderMode new_state)
{
	if (should_change(&states->render_mode, new_state))
	{
		const auto mode = ([new_state]() -> GLenum
			{
				switch (new_state)
				{
				case RenderMode::fill: return GL_FILL;
				case RenderMode::line: return GL_LINE;
				case RenderMode::point: return GL_POINT;
				default: DIE("Invalid render mode"); return GL_FILL;
				}
			})();
		glPolygonMode(GL_FRONT_AND_BACK, mode);
	}
	return *this;
}

StateChanger& StateChanger::stencil_op(StencilAction stencil_fail, StencilAction depth_fail, StencilAction pass)
{
	if (should_change(&states->stencil_op, {stencil_fail, depth_fail, pass}))
	{
		// todo(Gustav): look into using glStencilOpSeparate instead to specify front and back faces
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glStencilOpSeparate.xhtml
		glStencilOp(enum_from_sa(stencil_fail), enum_from_sa(depth_fail), enum_from_sa(pass));
	}

	return *this;
}

StateChanger& StateChanger::cull_face_mode(CullFace new_state)
{
	if (should_change(&states->cull_face_mode, new_state))
	{
		const auto mode = ([new_state]() -> GLenum
			{
				switch (new_state)
				{
				case CullFace::front: return GL_FRONT;
				case CullFace::back: return GL_BACK;
				case CullFace::front_and_back: return GL_FRONT_AND_BACK;
				default: DIE("Invalid cull face mode"); return GL_BACK;
				}
			})();
		glCullFace(mode);
	}
	return *this;
}

StateChanger& StateChanger::blend_mode(Blend src, Blend dst)
{
	if (should_change(&states->blend_mode, {src, dst}))
	{
		const auto convert = [](Blend b) -> GLenum
		{
			switch (b)
			{
			case Blend::zero: return GL_ZERO;
			case Blend::one: return GL_ONE;
			case Blend::src_color: return GL_SRC_COLOR;
			case Blend::one_minus_src_color: return GL_ONE_MINUS_SRC_COLOR;
			case Blend::dst_color: return GL_DST_COLOR;
			case Blend::one_minus_dst_color: return GL_ONE_MINUS_DST_COLOR;
			case Blend::src_alpha: return GL_SRC_ALPHA;
			case Blend::one_minus_src_alpha: return GL_ONE_MINUS_SRC_ALPHA;
			case Blend::dst_alpha: return GL_DST_ALPHA;
			case Blend::one_minus_dst_alpha: return GL_ONE_MINUS_DST_ALPHA;
			case Blend::constant_color: return GL_CONSTANT_COLOR;
			case Blend::one_minus_constant_color: return GL_ONE_MINUS_CONSTANT_COLOR;
			case Blend::constant_alpha: return GL_CONSTANT_ALPHA;
			case Blend::one_minus_constant_alpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
			case Blend::src_alpha_saturate: return GL_SRC_ALPHA_SATURATE;
			case Blend::src1_color: return GL_SRC1_COLOR;
			case Blend::one_minus_src1_color: return GL_ONE_MINUS_SRC1_COLOR;
			case Blend::src1_alpha: return GL_SRC1_ALPHA;
			case Blend::one_minus_src1_alpha: return GL_ONE_MINUS_SRC1_ALPHA;
			default: DIE("Invalid blend mode"); return GL_ZERO;
			}
		};
		glBlendFunc(convert(src), convert(dst));
	}
	return *this;
}

StateChanger& StateChanger::activate_texture(int new_texture)
{
	if (should_change(&states->active_texture, new_texture))
	{
		glActiveTexture(glenum_from_int(GL_TEXTURE0 + new_texture));
	}
	return *this;
}

StateChanger& StateChanger::bind_texture_2d(int slot, unsigned int texture)
{
	ASSERT(slot == states->active_texture);
	if (should_change(&states->texture_bound[sizet_from_int(slot)], texture))
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	return *this;
}

StateChanger& StateChanger::bind_texture_cubemap(int slot, unsigned int texture)
{
	ASSERT(slot == states->active_texture);
	if (should_change(&states->texture_bound[sizet_from_int(slot)], texture))
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	}
	return *this;
}

void bind_texture_2d(State* states, const Uniform& uniform, const Texture2d& texture)
{
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.texture >= 0);

	StateChanger{states}.activate_texture(uniform.texture).bind_texture_2d(uniform.texture, texture.id);
}

void bind_texture_2d(State* states, const Uniform& uniform, const FrameBuffer& texture)
{
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.texture >= 0);
	ASSERT(texture.debug_is_msaa == false);

	StateChanger{states}.activate_texture(uniform.texture).bind_texture_2d(uniform.texture, texture.id);
}

void bind_texture_cubemap(State* states, const Uniform& uniform, const TextureCubemap& texture)
{
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.texture >= 0);

	StateChanger{states}.activate_texture(uniform.texture).bind_texture_cubemap(uniform.texture, texture.id);
}

}  //  namespace eu::render
