#pragma once

// #include "render/color.h"

namespace eu::render
{
struct Geom;
}

namespace eu::render::geom
{

/** \addtogroup geom-builder
 *  @{
*/

/// A type alias for the index used in the \ref Builder.
/// Can reference a position, normal, texture coordinate, color or more.
using Index = u64;

/// An enum describing if a face is one or two-sided 
enum class SideCount
{
	one_sided,
	two_sided
};

/// The possible directions that normals can face.
enum class NormalsFacing
{
	In,
	Out
};

/// A single vertex referencing the shared data in the Builder
struct Vertex
{
	Index position;
	Index normal;
	Index texture;
	Index color;

	Vertex(Index pnt, Index clr);
	Vertex(Index a_position, Index a_normal, Index a_texture, Index a_color);
};

/// A simple triangle face in a Builder
/// \note Counter-clockwise order means a front face triangle
struct Triangle
{
	Vertex v0;
	Vertex v1;
	Vertex v2;

	Triangle(const Vertex& a, const Vertex& b, const Vertex& c);
};

/// A helper utility to create a Geom
struct Builder
{
	Index add_text_coord(const v2& tc);
	Index add_position(const v3& pos);
	Index add_normal(const v3& norm);
	Index add_color(const Lin_rgb& color);

	// find or add
	Index foa_text_coord(const v2& v, float max_diff);
	Index foa_position(const v3& pos, float max_diff);
	Index foa_normal(const v3& norm, float max_diff);
	Index foa_color(const Lin_rgb& color, float max_diff);

	Builder& add_triangle(const Triangle& t);
	Builder& add_quad(bool ccw, const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3);
	Builder& add_face(const std::vector<Vertex>&);

	Builder& move(const v3& dir);
	Builder& scale(float scale);
	Builder& invert_normals();

	[[nodiscard]] Geom to_geom() const;

	Builder& write_obj(const std::string& path);

	std::vector<std::vector<Vertex>> faces; // each face needs to have 3 or more vertices
	std::vector<v3> positions;
	std::vector<v3> normals;
	std::vector<v2> texcoords;
	std::vector<v3> lin_colors; ///< in linear space
};



/// @brief Creates a box/cube mesh.
/// @param x The width of the box (size along the x-axis).
/// @param y The height of the box (size along the y-axis).
/// @param z The depth of the box (size along the z-axis).
/// @param normals_facing Specifies the direction the box's normals are facing.
/// @param color The color of the box.
/// @return A \ref Builder representing the generated mesh.
Builder create_box(float x, float y, float z, NormalsFacing normals_facing, const Rgb& color = colors::white);



Builder create_xz_plane(float x, float z, bool invert, const Rgb& color = colors::white);



Builder create_xy_plane(float x, float y, SideCount two_sided, const Rgb& color = colors::white);



/// Creates a UV sphere mesh.
/// @param diameter The diameter of the sphere.
/// @param longitude_count The number of lines to go vertical, from pole to pole, aka meridians. Must be at least 3.
/// @param latitude_count The number of lines that go horizontal, parallels to the equator, which is one of those lines.
/// @param normals_facing The direction that the normals should face.
/// @param color The color to apply to the sphere.
/// @return A \ref Builder object representing the generated mesh.
Builder create_uv_sphere(
	float diameter, int longitude_count, int latitude_count, NormalsFacing normals_facing, const Rgb& color = colors::white
);

/**
 * @}
*/

}  //  namespace eu::render::geom
