#pragma once

namespace eu::core
{

/** \addtogroup geom Geom
 * \brief Vertices and triangles without a material
 *  @{
*/

/// A single vertex in a Geom
struct Vertex
{
	v3 position;
	v3 normal;
	v2 uv;
	v3 color; ///< in linear color space
};

/// A triangulated face in a Geom
struct Face
{
	u32 a;
	u32 b;
	u32 c;
};

/// vertices and triangles
struct Geom
{
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
};

/**
 * @}
*/

}  //  namespace eu::core
