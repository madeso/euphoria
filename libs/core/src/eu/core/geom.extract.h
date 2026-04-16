#pragma once

namespace eu::core
{
struct Geom;
struct CompiledGeomVertexAttributes;
}  //  namespace eu::core

namespace eu::core
{

/** \addtogroup geom-builder
 *  @{
*/

/// The type of a extracted attribute, like 'float'
enum class ExtractedAttributeType
{
	Float
};

/// A extracted attribute like `float 3`
struct ExtractedAttribute
{
	ExtractedAttributeType type;
	int count;
	std::size_t size;
};

/// Extracted data from a Geom for a specific shader layout so a CompiledGeom can be created
struct ExtractedGeom
{
	std::vector<char> data;
	std::size_t stride;

	std::vector<ExtractedAttribute> attributes;
	std::vector<u32> indices;
	i32 face_size;
};

ExtractedGeom extract_geom(const Geom& geom, const CompiledGeomVertexAttributes& layout);

/**
 * @}
*/

}  //  namespace eu::core
