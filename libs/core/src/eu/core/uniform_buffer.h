#pragma once


namespace eu::core
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

/**
 * @}
*/

}  //  namespace eu::render
