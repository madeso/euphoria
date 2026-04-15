#include "eu/log/log.h"

#include "eu/render/fullscreen.h"
#include "eu/render/renderer.pimpl.h"

#include "glad/glad.h"
#include "eu/render/shader_resource.h"

namespace eu::render
{


std::string string_from_gl_bytes(const GLubyte* bytes)
{
	return reinterpret_cast<const char*>(bytes);
}

CameraUniformBuffer make_camera_uniform_buffer_desc()
{
	CameraUniformBuffer camera_uniform_buffer;

	{
		UniformBufferCompiler compiler;
		compiler.add(&camera_uniform_buffer.clip_from_view_uni, UniformType::mat4, "u_clip_from_view");
		compiler.add(&camera_uniform_buffer.view_from_world_uni, UniformType::mat4, "u_view_from_world");
		camera_uniform_buffer.setup = compiler.compile("Camera", 0);
	}

	camera_uniform_buffer.buffer = std::make_unique<UniformBuffer>(USE_DEBUG_LABEL_MANY("camera uniform buffer") camera_uniform_buffer.setup);

	return camera_uniform_buffer;
}

RendererPimpl::RendererPimpl(State* new_states, const Assets& assets, const RenderSettings& set, const FullScreenGeom& full_screen)
	: states(new_states)
    , camera_uniform_buffer(make_camera_uniform_buffer_desc())
	, shaders_resources(load_shaders(assets, camera_uniform_buffer, set, full_screen))
	, full_screen_geom(full_screen.geom)
{
	const auto vendor = string_from_gl_bytes(glGetString(GL_VENDOR));
	const auto renderer = string_from_gl_bytes(glGetString(GL_RENDERER));
	const auto version = string_from_gl_bytes(glGetString(GL_VERSION));
	const auto shading_language_version = string_from_gl_bytes(glGetString(GL_SHADING_LANGUAGE_VERSION));
	const auto extensions = string_from_gl_bytes(glGetStringi(GL_EXTENSIONS, 0));

	LOG_INFO("vendor {}, renderer {}", vendor.c_str(), renderer.c_str());
	LOG_INFO("version {} (glsl {})", version.c_str(), shading_language_version.c_str());
	LOG_INFO("extensions {}", extensions.c_str());
}


}  //  namespace eu::render
