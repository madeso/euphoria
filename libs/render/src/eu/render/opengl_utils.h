#pragma once

#include "dependency_glad.h"
#include "eu/base/size.h"

namespace eu::render
{


void setup_opengl_debug();

GLenum glenum_from_int(int i);
GLuint gluint_from_int(int i);
GLsizeiptr glsizeiptr_from_sizet(std::size_t t);
GLsizei glsizei_from_sizet(std::size_t t);

u32 create_buffer();
void destroy_buffer(u32 vbo);

u32 create_vertex_array();
void destroy_vertex_array(u32 vao);

void set_gl_viewport(const Size& sz);

#if FF_HAS(ENABLE_GL_DEBUG)

	enum class DebugLabelFor
	{
	    Buffer,
	    Shader,
	    Program,
	    VertexArray,
	    Query,
	    ProgramPipeline,
	    Sampler,
	    Texture,
	    RenderBuffer,
	    FrameBuffer
	};

	void set_gl_debug_label(DebugLabelFor type, GLuint object, const std::string& label);
	void set_gl_debug_label(DebugLabelFor type, GLuint object, std::string_view label);


	#define SET_DEBUG_LABEL(ID, FOR) set_gl_debug_label(FOR, ID, debug_label)
	#define SET_DEBUG_LABEL_NAMED(ID, FOR, NAME) set_gl_debug_label(FOR, ID, NAME)

	// See opengl_labels.h for passing the labels/names

	struct ScopedDebugGroup
	{
		explicit ScopedDebugGroup(const std::string& message, unsigned int id=0);
		explicit ScopedDebugGroup(std::string_view message, unsigned int id=0);
	    ~ScopedDebugGroup();

	    ScopedDebugGroup(ScopedDebugGroup&&) = delete;
		ScopedDebugGroup(const ScopedDebugGroup&) = delete;
	    void operator=(ScopedDebugGroup&&) = delete;
		void operator=(const ScopedDebugGroup&) = delete;
	};

	#define CONCAT_IMPL(x, y) x##y
	#define CONCAT(x, y) CONCAT_IMPL(x, y)
	#define SCOPED_DEBUG_GROUP(TEXT) [[maybe_unused]] const ScopedDebugGroup CONCAT(sc, __LINE__){TEXT}

#else
	constexpr void opengl_nop()
	{
	}

	#define SET_DEBUG_LABEL(ID, FOR) opengl_nop()
	#define SET_DEBUG_LABEL_NAMED(ID, FOR, NAME) opengl_nop()

	#define SCOPED_DEBUG_GROUP(TEXT) opengl_nop()
#endif


}
