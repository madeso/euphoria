#pragma once

#include "render/shader.h"

namespace eu::render
{

struct CompiledCamera;
// struct Lin_rgb; // in base/colors.h

/// A utility to draw 3d lines.
/// Helps to batch lines for rendering.
struct LineDrawer
{
	static constexpr int max_lines = 100;

	ShaderVertexAttributes description;
	CompiledShaderVertexAttributes layout;
	ShaderProgram shader;
	Uniform clip_from_view_uni;
	Uniform view_from_world_uni;
	Uniform resolution_uni;
	Uniform dash_size_uni;
	Uniform gap_size_uni;

	std::vector<float> data;
	int lines = 0;
	u32 va;
	u32 vb;
	u32 ib;

	LineDrawer();
	~LineDrawer();

	LineDrawer(const LineDrawer&) = delete;
	void operator=(const LineDrawer&) = delete;
	LineDrawer(LineDrawer&&) = delete;
	void operator=(LineDrawer&&) = delete;

	void set_camera(const CompiledCamera& cc);
	void set_line_to_dash(const v2& resolution, float dash_size, float gap_size);
	void set_line_to_solid();
	void line(const v3& world_from, const v3& world_to, const Lin_rgb& color);
	void submit();

	[[nodiscard]] bool is_loaded() const;
};

}  //  namespace eu::render
