#include "render/geom.h"

#include "assert/assert.h"
#include "base/cint.h"

#include "render/geom.extract.h"
#include "render/vertex_layout.h"

#include <numeric>

namespace eu::render
{

using ByteBuffer = std::vector<char>;

std::vector<u32> compile_indices(const Geom& geom)
{
	std::vector<u32> indices;
	const auto push = [&indices](const u32 i)
	{
		indices.emplace_back(i);
	};
	for (const auto& f: geom.faces)
	{
		push(f.a);
		push(f.b);
		push(f.c);
	}
	return indices;
}

void push_float(ByteBuffer* vv, float f)
{
	// todo(Gustav): is this safe?
	union
	{
		float f;
		char bytes[4];
	} convert {};

	convert.f = f;
	for (int byte_index = 0; byte_index < 4; byte_index += 1)
	{
		vv->emplace_back(convert.bytes[byte_index]);
	}
}

void push2(ByteBuffer* vv, const v2& p)
{
	push_float(vv, p.x);
	push_float(vv, p.y);
}

void push3(ByteBuffer* vv, const v3& p)
{
	push_float(vv, p.x);
	push_float(vv, p.y);
	push_float(vv, p.z);
}

void push4(ByteBuffer* vv, const v4& p)
{
	push_float(vv, p.x);
	push_float(vv, p.y);
	push_float(vv, p.z);
	push_float(vv, p.w);
}

struct BufferData
{
	using PerVertex = void (*)(ByteBuffer*, const Vertex&);

	std::vector<ExtractedAttribute> attributes;
	std::vector<PerVertex> per_vertex;
};

ExtractedGeom extract_geom(const Geom& geom, const CompiledGeomVertexAttributes& layout)
{
	const auto indices = compile_indices(geom);

	const BufferData attribute_descriptions = [&]()
	{
		auto data = BufferData{};
		data.attributes.reserve(layout.elements.size());
		data.per_vertex.reserve(layout.elements.size());

		for (const auto& element: layout.elements)
		{
			switch (element.type)
			{
#define MAP(VT, PROP, COUNT) \
	case VT: \
		data.attributes.emplace_back(ExtractedAttribute{ExtractedAttributeType::Float, COUNT, sizeof(float) * (COUNT)}); \
		data.per_vertex.emplace_back([](ByteBuffer* vertices, const Vertex& vertex) { push##COUNT(vertices, PROP); }); \
		break
				MAP(VertexType::position2xy, (v2{vertex.position.x, vertex.position.y}), 2);
				MAP(VertexType::position2xz, (v2{vertex.position.x, vertex.position.z}), 2);
				MAP(VertexType::position3, vertex.position, 3);
				MAP(VertexType::normal3, vertex.normal, 3);
				MAP(VertexType::color3, vertex.color, 3);
				MAP(VertexType::color4, v4(vertex.color, 1.0f), 4);
				MAP(VertexType::texture2, vertex.uv, 2);
#undef MAP
			case VertexType::instance_transform:
				DIE("can't use instance types for extraction");
				break;	// todo(Gustav): should something else be done here?
			default: DIE("Invalid buffer type"); break;
			}
		}
		return data;
	}();

	const std::size_t bytes_per_vertex = std::accumulate(
		attribute_descriptions.attributes.begin(),
		attribute_descriptions.attributes.end(),
		static_cast<std::size_t>(0),
		[](std::size_t s, const ExtractedAttribute& d) -> std::size_t { return s + d.size; }
	);
	const auto vertices = [&]()
	{
		auto verts = ByteBuffer{};
		verts.reserve(geom.vertices.size() * bytes_per_vertex);
		for (const auto& vertex: geom.vertices)
		{
			for (const auto& per_vertex: attribute_descriptions.per_vertex)
			{
				per_vertex(&verts, vertex);
			}
		}
		return verts;
	}();

	const auto face_size = static_cast<i32>(geom.faces.size());

	return {vertices, bytes_per_vertex, attribute_descriptions.attributes, indices, face_size};
}

}  //  namespace eu::render
