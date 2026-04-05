#pragma once
#include "imgui.h"

namespace eu::render
{
    struct FrameBuffer;
}

namespace eu::imgui_extra
{

struct ImguiShaderProgram
{
	unsigned int program_handle = 0;
	int texture_attrib = 0;
	int projection_attrib = 0;
};

struct ImguiShaderCache
{
	ImguiShaderCache();
	~ImguiShaderCache();

	ImguiShaderCache(const ImguiShaderCache&) = delete;
	ImguiShaderCache(ImguiShaderCache&&) = delete;
	ImguiShaderCache& operator=(const ImguiShaderCache&) = delete;
	ImguiShaderCache& operator=(ImguiShaderCache&&) = delete;

	ImguiShaderProgram linear_to_gamma_shader;
	ImguiShaderProgram depth_ortho_shader;
};

enum class ImageShader
{
	None,
	TonemapAndGamma,
	DepthOrtho
};


void imgui_text(const std::string& str);
void imgui_image(const char* name, const render::FrameBuffer& img, ImguiShaderCache* cache, ImageShader shader);
bool simple_gamma_slider(const char* label, float* gamma, float curve = 1.5f, float min_gamma = 1.6f, float max_gamma = 2.8f);

}
