#include "render/fullscreen.h"

#include "render/geom.h"
#include "render/geom.builder.h"


namespace eu::render
{

FullScreenGeom::FullScreenGeom()
{
	const auto layout_shader_material = ShaderVertexAttributes{
		{VertexType::position2xy, "a_position"},
		{VertexType::texture2, "a_tex_coord"}
	};
	const auto layout_compiler = compile_attribute_layouts({layout_shader_material});
	const auto geom_layout = get_geom_layout(layout_compiler);

	// ndc is [-1, 1], plane func generate a centered geom, so we set our plane to a size of 2
	constexpr auto plane_size = 2.0f;
	
	layout = compile_shader_layout(layout_compiler, layout_shader_material, std::nullopt, std::nullopt);
	geom = compile_geom(
		USE_DEBUG_LABEL_MANY("full screen geom") geom::create_xy_plane(plane_size, plane_size, geom::SideCount::one_sided).to_geom(),
		geom_layout
	);
}

}  //  namespace eu::render
