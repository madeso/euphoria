#include "render/shader.h"

#include "base/cint.h"

#include "render/constants.h"
#include "base/mat4.h"
#include "base/vec2.h"
#include "render/opengl_utils.h"
#include "render/uniform_buffer.h"

namespace eu::render
{

// internal "header", defined later
void set_shader_program(unsigned int new_program, const VertexTypes& types);

constexpr GLsizei max_log_length = 1024;

bool check_shader_compilation_error(const char* name, unsigned int shader)
{
	GLint success = 0;
	char log[max_log_length] = {
		0,
	};
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(shader, max_log_length, nullptr, log);
		log[max_log_length-1] = '\0';
		LOG_ERR("%s shader compilation failed\n{}\n", name, log);
		return false;
	}

	return true;
}

bool check_shader_link_error(unsigned int program)
{
	GLint success = 0;
	char log[max_log_length] = {
		0,
	};
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		glGetProgramInfoLog(program, max_log_length, nullptr, log);
		log[max_log_length - 1] = '\0';
		LOG_ERR("shader linking failed\n{}\n", log);
		return false;
	}

	return true;
}

void upload_shader_source(unsigned int shader, std::string_view source)
{
	const char* const s = source.data();
	const int length = int_from_sizet(source.length());	// should be GLint
	glShaderSource(shader, 1, &s, &length);
}

void bind_shader_attribute_location(unsigned int shader_program, const CompiledShaderVertexAttributes& layout)
{
	for (const auto& b: layout.elements)
	{
		glBindAttribLocation(shader_program, gluint_from_int(b.index), b.name.c_str());
	}
}

void verify_shader_attribute_location(unsigned int shader_program, const CompiledShaderVertexAttributes& layout)
{
	for (const auto& b: layout.elements)
	{
		const auto actual_index = glGetAttribLocation(shader_program, b.name.c_str());

		if (actual_index != b.index)
		{
			LOG_ERR("ERROR: {} was bound to {} but requested at {}", b.name.c_str(), actual_index, b.index);
		}
	}
}

void log_source(const std::string& src)
{
	std::istringstream ss(src);
	std::string line;
	int index = 1;
	while (std::getline(ss, line))
	{
		LOG_INFO("%d: %s", index, line.c_str());
		index += 1;
	}
}

void load_shader_source(
	DEBUG_LABEL_ARG_MANY
	ShaderProgram* self,
	const std::string& vertex_source,
	const std::string& fragment_source,
	const CompiledShaderVertexAttributes& layout
)
{
	const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	SET_DEBUG_LABEL_NAMED(vertex_shader, DebugLabelFor::Shader, fmt::format("SHADER {} VERT", debug_label));
	upload_shader_source(vertex_shader, vertex_source);
	glCompileShader(vertex_shader);
	const auto vertex_ok = check_shader_compilation_error("vertex", vertex_shader);
	if (vertex_ok == false)
	{
		LOG_INFO("Vertex source:");
		log_source(vertex_source);
	}

	const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	SET_DEBUG_LABEL_NAMED(fragment_shader, DebugLabelFor::Shader, fmt::format("SHADER {} FRAG", debug_label));
	upload_shader_source(fragment_shader, fragment_source);
	glCompileShader(fragment_shader);
	const auto fragment_ok = check_shader_compilation_error("fragment", fragment_shader);
	if (fragment_ok == false)
	{
		LOG_INFO("Fragment source:");
		log_source(fragment_source);
	}

	glAttachShader(self->shader_program, vertex_shader);
	glAttachShader(self->shader_program, fragment_shader);
	bind_shader_attribute_location(self->shader_program, layout);
	glLinkProgram(self->shader_program);
	const auto link_ok = check_shader_link_error(self->shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	clear_shader_program();

	if (vertex_ok && fragment_ok && link_ok)
	{
		// nop
		verify_shader_attribute_location(self->shader_program, layout);
	}
	else
	{
		clear_shader_program();
		glDeleteProgram(self->shader_program);
		self->shader_program = 0;
	}
}

ShaderProgram::ShaderProgram(
	DEBUG_LABEL_ARG_MANY const std::string& vertex_source, const std::string& fragment_source, const CompiledShaderVertexAttributes& layout
)
	: shader_program(glCreateProgram())
	, debug_vertex_types(layout.debug_types)
{
	SET_DEBUG_LABEL_NAMED(shader_program, DebugLabelFor::Program, fmt::format("PROGRAM {}", debug_label));
	load_shader_source(USE_DEBUG_LABEL_MANY(debug_label) this, vertex_source, fragment_source, layout);
}

void ShaderProgram::use() const
{
	set_shader_program(shader_program, debug_vertex_types);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
	: shader_program(other.shader_program)
	, debug_vertex_types(std::move(other.debug_vertex_types))
{
	other.shader_program = 0;
	other.debug_vertex_types = {};
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& rhs) noexcept
{
	clear();

	shader_program = rhs.shader_program;
	debug_vertex_types = rhs.debug_vertex_types;

	rhs.shader_program = 0;
	rhs.debug_vertex_types = {};

	return *this;
}

ShaderProgram::~ShaderProgram()
{
	clear();
}

bool ShaderProgram::is_loaded() const
{
	return shader_program != 0;
}

void ShaderProgram::clear()
{
	clear_shader_program();
	glDeleteProgram(shader_program);
	shader_program = 0;
}

Uniform ShaderProgram::get_uniform(const std::string& name) const
{
	const auto uni = Uniform{name, glGetUniformLocation(shader_program, name.c_str()), shader_program};
	if (uni.is_valid() == false)
	{
		LOG_ERR("Uniform {} not found", name);
	}
	return uni;
}

void ShaderProgram::set_float(const Uniform& uniform, float value) // NOLINT(readability-make-member-function-const)
{
	ASSERT(is_shader_bound(shader_program));
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.debug_shader_program == shader_program);

	ASSERT(uniform.texture == -1 && "uniform is a texture not a float");
	glUniform1f(uniform.location, value);
}

void ShaderProgram::set_bool(const Uniform& uniform, bool value)  // NOLINT(readability-make-member-function-const)
{
	ASSERT(is_shader_bound(shader_program));
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.debug_shader_program == shader_program);

	ASSERT(uniform.texture == -1 && "uniform is a texture not a float");
	glUniform1i(uniform.location, value ? 1 : 0);
}

void ShaderProgram::set_vec2(const Uniform& uniform, float x, float y) // NOLINT(readability-make-member-function-const)
{
	ASSERT(is_shader_bound(shader_program));
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.debug_shader_program == shader_program);

	ASSERT(uniform.texture == -1 && "uniform is a texture not a vec3");
	glUniform2f(uniform.location, x, y);
}

void ShaderProgram::set_vec2(const Uniform& uniform, const v2& v)
{
	set_vec2(uniform, v.x, v.y);
}

void ShaderProgram::set_vec3(const Uniform& uniform, float x, float y, float z) // NOLINT(readability-make-member-function-const)
{
	ASSERT(is_shader_bound(shader_program));
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.debug_shader_program == shader_program);

	ASSERT(uniform.texture == -1 && "uniform is a texture not a vec3");
	glUniform3f(uniform.location, x, y, z);
}

void ShaderProgram::set_vec3(const Uniform& uniform, const v3& v)
{
	set_vec3(uniform, v.x, v.y, v.z);
}

void ShaderProgram::set_vec4(const Uniform& uniform, float x, float y, float z, float w) // NOLINT(readability-make-member-function-const)
{
	ASSERT(is_shader_bound(shader_program));
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.debug_shader_program == shader_program);

	ASSERT(uniform.texture == -1 && "uniform is a texture not a vec4");
	glUniform4f(uniform.location, x, y, z, w);
}

void ShaderProgram::set_vec4(const Uniform& uniform, const v4& v)
{
	set_vec4(uniform, v.x, v.y, v.z, v.w);
}

void ShaderProgram::set_texture(const Uniform& uniform) // NOLINT(readability-make-member-function-const)
{
	ASSERT(is_shader_bound(shader_program));
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.debug_shader_program == shader_program);

	ASSERT(uniform.texture >= 0 && "uniform needs to be a texture");
	glUniform1i(uniform.location, uniform.texture);
}

void ShaderProgram::set_mat(const Uniform& uniform, const m4& mat) // NOLINT(readability-make-member-function-const)
{
	ASSERT(is_shader_bound(shader_program));
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.debug_shader_program == shader_program);

	ASSERT(uniform.texture == -1 && "uniform is a texture not a matrix4");
	glUniformMatrix4fv(uniform.location, 1, GL_FALSE, mat.get_column_major_data_ptr());
}

void ShaderProgram::setup_uniform_block(const UniformBufferSetup& setup) // NOLINT(readability-make-member-function-const)
{
	const unsigned int shader_block_index = glGetUniformBlockIndex(shader_program, setup.name.c_str());
	if (shader_block_index == GL_INVALID_INDEX)
	{
		LOG_ERR("Shader missing uniform block %s", setup.name.c_str());
		return;
	}
	glUniformBlockBinding(shader_program, shader_block_index, gluint_from_int(setup.binding_point));
}

void setup_textures(ShaderProgram* shader, const std::vector<Uniform*>& uniform_list)
{
	ASSERT(uniform_list.size() <= MAX_TEXTURES_SUPPORTED);

	shader->use();

	int index = 0;
	for (const auto& uniform: uniform_list)
	{
		uniform->texture = index;
		index += 1;
		shader->set_texture(*uniform);
	}
}

namespace
{
	VertexTypes debug_current_shader_types;
	unsigned int debug_current_shader_program = 0;
}  //  namespace

void set_shader_program(unsigned int new_program, const VertexTypes& types)
{
	debug_current_shader_program = new_program;
	debug_current_shader_types = types;
	glUseProgram(new_program);
}

bool is_bound_for_shader(const std::unordered_set<VertexType>& debug_geom_shader_types)
{
	for (auto t: debug_current_shader_types)
	{
		if (debug_geom_shader_types.find(t) == debug_geom_shader_types.end())
		{
			// if shader type isn't found in geom
			// then error out
			return false;
		}
	}
	return true;
}

bool is_shader_bound(unsigned int program)
{
	return debug_current_shader_program == program;
}

void clear_shader_program()
{
	set_shader_program(0, {});
}

}
