#pragma once

#include "eu/render/texture.h"

#include "embed/types.h"

namespace eu::render
{

[[nodiscard]]
Texture2d load_image_from_embedded(
	DEBUG_LABEL_ARG_MANY 
	const embedded_binary& image_binary, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd
);

/**
@param images images in the following order: right, left, top, bottom, back and front
@param cd How to load/interpret the color from the image
 */
[[nodiscard]]
TextureCubemap load_cubemap_from_embedded(
	DEBUG_LABEL_ARG_MANY 
	const std::array<embedded_binary, cubemap_size>& images,
	ColorData cd
);


}
