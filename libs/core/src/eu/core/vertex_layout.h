#pragma once

#include <map>
#include <set>
#include <optional>

namespace eu::core
{

/** \defgroup vertex-layout Vertex layout
 * \brief Functions and types related to transforming generic mesh data to renderable data used by shaders.
 *  @{
*/

/// Vertex source type, position, normal etc.
/// @todo change to include other texture coordinates and custom types that are created from scripts
enum class VertexType
{
	// vertex based
	position2xy,
	position2xz,
	position3,
	normal3,
	color3,
	color4,
	texture2,

	// instance based
	instance_transform
};

constexpr bool is_instance_based(VertexType v)
{
	switch (v)
	{
	case VertexType::instance_transform: return true;
	default: return false;
	}
}

/// A not-yet-realised binding to a shader variable like `vec3 position`
struct VertexElementDescription
{
	VertexType type;
	std::string name;
};

/// Describes all vertex inputs a shader requires like `[vec3 position, vec3 normal, vec2 uv]`
/// \note It's invalid for this to contain duplicates, use \ref find_duplicates to find duplicates
using ShaderVertexAttributes = std::vector<VertexElementDescription>;

std::vector<VertexType> find_duplicates(const ShaderVertexAttributes& items);

/// A realized VertexElementDescription like `vec3 position` at gl shader position 3
struct CompiledVertexElement
{
	VertexType type;
	std::string name;
	int index;
};

/// A realized VertexElementDescription without name like `vec3` at gl shader position 3
struct CompiledVertexElementNoName
{
	VertexType type;
	int index;
};

using VertexTypes = std::vector<VertexType>;

/// A list of CompiledVertexElement (for shader)
struct CompiledShaderVertexAttributes
{
	std::vector<CompiledVertexElement> elements;
	VertexTypes debug_types;
};

/// A list of CompiledVertexLayoutNoNameList (for geom)
struct CompiledGeomVertexAttributes
{
	std::vector<CompiledVertexElementNoName> elements;
	VertexTypes debug_types;
};

/// A mapping of the vertex type (position...) to the actual shader id (for more than one shader)
struct CompiledVertexTypeList
{
	std::map<VertexType, int> indices;
	VertexTypes debug_types;
	int next_index;
};

/// A property for a shader used in instanced rendering.
/// @see \ref compile_shader_layout
struct InstanceProp
{
	VertexType type;
	std::string name;
};

[[nodiscard]]
CompiledShaderVertexAttributes compile_shader_layout(
	const CompiledVertexTypeList& l,
	const ShaderVertexAttributes& elements,
	std::optional<InstanceProp> instance_prop,
	std::optional<int> start_index
);

[[nodiscard]]
CompiledGeomVertexAttributes get_geom_layout(const CompiledVertexTypeList& l);

CompiledVertexTypeList compile_attribute_layouts(
	const std::vector<VertexType>& base_layout, const std::vector<ShaderVertexAttributes>& descriptions
);

CompiledVertexTypeList compile_attribute_layouts(const std::vector<ShaderVertexAttributes>& descriptions);

/** @}
*/

}  //  namespace eu::core
