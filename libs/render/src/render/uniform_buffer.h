#pragma once

#include "render/opengl_labels.h"

namespace eu
{
    struct m4;
}

namespace eu::render
{

/** \addtogroup render Renderer
 *  @{
*/

enum class UniformType
{
	invalid,
	bool_type,
	int_type,
	float_type,
	vec2,
	vec3,
	vec4,
	mat4
};

struct UniformBufferSetup
{
	int size = 512;
	int binding_point = 1;
	std::string name;
	std::string source;
};

/// A compiled uniform buffer property that can be used in a shader.
struct CompiledUniformProp
{
	int offset = -1;
	UniformType type = UniformType::invalid;
	int array_count = -1;
};

/// internal
/// @see \ref UniformBufferCompiler
struct UniformProp
{
	CompiledUniformProp* target;
	UniformType type;
	std::string name;
	int array_count;
};

struct UniformBufferCompiler
{
	void add(CompiledUniformProp* target, UniformType type, const std::string& name, int array_count = 1);

	[[nodiscard]]
	UniformBufferSetup compile(const std::string& name, int binding_point) const;

	std::vector<UniformProp> props;
};

struct UniformBuffer
{
	DEBUG_LABEL_EXPLICIT_MANY UniformBuffer(DEBUG_LABEL_ARG_MANY const UniformBufferSetup& setup);
	~UniformBuffer();

	void set_mat4(const CompiledUniformProp& prop, const m4& m);

	UniformBuffer(const UniformBuffer&) = delete;
	void operator=(const UniformBuffer&) = delete;

	UniformBuffer(UniformBuffer&&) noexcept;
	UniformBuffer& operator=(UniformBuffer&&) noexcept;

	// clears the loaded buffer to a invalid buffer
	void unload();

	unsigned int id;
};

struct BoundUniformBuffer
{
	explicit BoundUniformBuffer(UniformBuffer* b);
	~BoundUniformBuffer();

	BoundUniformBuffer(const BoundUniformBuffer&) = delete;
	BoundUniformBuffer(BoundUniformBuffer&&) = delete;
	void operator=(const BoundUniformBuffer&) = delete;
	void operator=(BoundUniformBuffer&&) = delete;

	UniformBuffer* buffer;
};

/**
 * @}
*/

}  //  namespace eu::render
