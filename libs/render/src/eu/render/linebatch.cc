#include "eu/render/linebatch.h"

#include "eu/assert/assert.h"

#include "eu/render/opengl_utils.h"
#include "eu/render/camera.h"

#include "eu/render/dependency_glad.h"

constexpr auto vertices_per_line = 2;

namespace eu::render
{
LineDrawer::LineDrawer()
	// todo(Gustav): expand shader with stipple pattern https://stackoverflow.com/questions/52928678/dashed-line-in-opengl3/54543267#54543267
	// todo(Gustav): move `description` and `layout` to a separate setup
	: description({{VertexType::position3, "a_world_position"}, {VertexType::color3, "a_color"}})
	, layout(compile_shader_layout(compile_attribute_layouts({description}), description, std::nullopt, std::nullopt))
	, shader(
		  USE_DEBUG_LABEL_MANY("debug line")
		  R"glsl(
			#version 430 core
			in vec3 a_world_position;
			in vec3 a_color;

			uniform mat4 u_clip_from_view;
			uniform mat4 u_view_from_world;

			out vec4 v_color;
			out vec4 v_vert_pos;
			flat out vec4 v_start_pos;

			void main()
			{
				v_color = vec4(a_color, 1.0);
				vec4 pos = u_clip_from_view * u_view_from_world * vec4(a_world_position.xyz, 1.0);
				gl_Position = pos;
				v_vert_pos = pos;
				v_start_pos = v_vert_pos;
			}
		)glsl",
		  R"glsl(
			#version 430 core

			in vec4 v_color;
			flat in vec4 v_start_pos;
			in vec4 v_vert_pos;

			uniform float u_dash_size;
			uniform float u_gap_size;
			uniform vec2  u_resolution;

			out vec4 color;

			vec2 pixel(vec4 pos)
			{
				return ((pos.xy / pos.w) + 1)/2.0 * u_resolution;
			}

			void main()
			{
				if(u_resolution.x > 0.0)
				{
					float seg_len = u_dash_size + u_gap_size;

					float dash_seg = u_dash_size/seg_len;

					float cur = length(pixel(v_vert_pos) - pixel(v_start_pos));
					float cur_seg = fract(cur / seg_len);
					if(cur_seg > dash_seg)
					{
						discard;
					}
					color = v_color;
				}
				else
				{
					color = v_color;
				}
			}
		)glsl",
		  layout
	  )
	, clip_from_view_uni(shader.get_uniform("u_clip_from_view"))
	, view_from_world_uni(shader.get_uniform("u_view_from_world"))
	, resolution_uni(shader.get_uniform("u_resolution"))
	, dash_size_uni(shader.get_uniform("u_dash_size"))
	, gap_size_uni(shader.get_uniform("u_gap_size"))
	, va(create_vertex_array())
	, vb(create_buffer())
	, ib(create_buffer())
{
	shader.use();

	glBindVertexArray(va);
	SET_DEBUG_LABEL_NAMED(va, DebugLabelFor::VertexArray, "VERT line batch"sv);

	constexpr auto attributes_per_vertex = 2;
	constexpr auto float_per_attribute = 3;

	constexpr auto floats_per_vertex = float_per_attribute * attributes_per_vertex;
	constexpr auto vertex_size = floats_per_vertex * sizeof(float);
	constexpr auto max_vertices = vertices_per_line * max_lines;
	constexpr auto max_indices = vertices_per_line * max_lines;

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	SET_DEBUG_LABEL_NAMED(vb, DebugLabelFor::Buffer, "ARR BUF line batch"sv);
	glBufferData(GL_ARRAY_BUFFER, vertex_size * max_vertices, nullptr, GL_DYNAMIC_DRAW);

	auto relative_offset = [](unsigned int i)
	{
		return reinterpret_cast<void*>(i * sizeof(float));
	};

	unsigned int offset = 0;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
	offset += 3;

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
	offset += 3;
	ASSERT(offset == floats_per_vertex);

	std::vector<u32> indices;
	indices.reserve(max_indices);

	for (auto quad_index = 0; quad_index < max_lines; quad_index += 1)
	{
		const auto base = quad_index * vertices_per_line;
		indices.emplace_back(base + 0);
		indices.emplace_back(base + 1);
	}

	ASSERT(max_indices == indices.size());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	SET_DEBUG_LABEL_NAMED(ib, DebugLabelFor::Buffer, "IND BUF line batch"sv);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices * sizeof(u32), indices.data(), GL_STATIC_DRAW);
}

LineDrawer::~LineDrawer()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	destroy_buffer(ib);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	destroy_buffer(vb);

	glBindVertexArray(0);
	destroy_vertex_array(va);
}

void LineDrawer::set_line_to_dash(const v2& resolution, float dash_size, float gap_size)
{
	submit();
	shader.set_vec2(resolution_uni, resolution);
	shader.set_float(dash_size_uni, dash_size);
	shader.set_float(gap_size_uni, gap_size);
}

void LineDrawer::set_line_to_solid()
{
	submit();
	shader.set_vec2(resolution_uni, {-1.0f, -1.0f});
}

bool LineDrawer::is_loaded() const
{
	return shader.is_loaded();
}

void LineDrawer::line(const v3& world_from, const v3& world_to, const Lin_rgb& color)
{
	if (lines == max_lines)
	{
		submit();
	}

	lines += 1;

	const auto add_vertex = [this,color](const v3& world_position)
	{
		data.push_back(world_position.x);
		data.push_back(world_position.y);
		data.push_back(world_position.z);

		data.push_back(color.r);
		data.push_back(color.g);
		data.push_back(color.b);
	};

	add_vertex(world_from);
	add_vertex(world_to);
}

void LineDrawer::set_camera(const CompiledCamera& compiled_camera)
{
	shader.set_mat(clip_from_view_uni, compiled_camera.clip_from_view);
	shader.set_mat(view_from_world_uni, compiled_camera.view_from_world);
}

void LineDrawer::submit()
{
	if (lines == 0)
	{
		return;
	}

	// note: this assumes set_camera has been called

	glBindVertexArray(va);

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferSubData(
		GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(sizeof(float) * data.size()), static_cast<const void*>(data.data())
	);
	glDrawElements(GL_LINES, vertices_per_line * lines, GL_UNSIGNED_INT, nullptr);

	data.resize(0);
	lines = 0;
}

}  //  namespace eu::render
