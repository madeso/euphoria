#pragma once

#include "eu/base/size.h"

#include "eu/render/opengl_labels.h"

#include "eu/core/color.h"

namespace eu::render
{

/** \addtogroup texture
 *  @{
*/

enum class TextureEdge
{
	clamp,
	repeat
};


enum class TextureRenderStyle
{
	pixel,
	mipmap,
	linear
};


enum class Transparency
{
	include,
	exclude
};

enum class ColorData
{
	/// Apply transformations to the color data. Texture is a diffuse/albedo map, created with human eyes on a monitor in sRGB space.
	color_data,

	// todo(Gustav): should this be more specific so we could define better default colors if the load fails?
	/// Don't apply transformations to the color data. Texture is a normal/roughness/ao/specular map or similar and created by a software in linear space.
	non_color_data,

	/// Explicitly don't care about the color data but same as \ref non_color_data.
	dont_care
};


// todo(Gustav): this doesn't do anything except allow code reuse, remove?
/// Base class for all textures, but only exist due to code reuse and can easily be inlined.
struct BaseTexture
{
	unsigned int id;

	BaseTexture();
	~BaseTexture();

	BaseTexture(const BaseTexture&) = delete;
	void operator=(const BaseTexture&) = delete;

	BaseTexture(BaseTexture&&) noexcept;
	BaseTexture& operator=(BaseTexture&&) noexcept;

	/// clears the loaded texture to a invalid texture
	void unload();
};

/// A 2d image texture.
struct Texture2d : BaseTexture
{
	Texture2d() = delete;

	/// "internal"
	Texture2d(DEBUG_LABEL_ARG_MANY const void* pixel_data, unsigned int pixel_format, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd);
};

Texture2d load_image_from_color(DEBUG_LABEL_ARG_MANY core::SingleColor pixel, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd);

// todo(Gustav): turn into an enum?
/// 0=right(x+), 1=left(x-), 2=top(y+), 3=bottom(y-), 4=front(z+), 5=back(z-)
constexpr std::size_t cubemap_size = 6;

/// A cubemap texture.
/// Useful for skybox rendering or faking reflections from a (static) scene.
struct TextureCubemap : BaseTexture
{
	TextureCubemap() = delete;

	TextureCubemap(DEBUG_LABEL_ARG_MANY const std::array<void*, cubemap_size>& pixel_data, int width, int height, ColorData cd);
};

TextureCubemap load_cubemap_from_color(DEBUG_LABEL_ARG_MANY core::SingleColor pixel, ColorData cd);



/// "render to texture" feature
///	@see \ref create-framebuffer
struct FrameBuffer : BaseTexture
{
	/// @param f The FBO handle
	/// @param s the texture size
	FrameBuffer(unsigned int f, const Size& s);
	~FrameBuffer();

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = delete;
	void operator=(const FrameBuffer&) = delete;
	void operator=(FrameBuffer&&) = delete;

	Size size;

	unsigned int fbo = 0;
	unsigned int rbo = 0;

	bool debug_is_msaa = false;
};

/// The number of bits to use for the depth buffer.
enum class DepthBits
{
	use_none,
	use_16, use_24, use_32
};

/// The number of bits/pixel to use for the color buffer.
enum class ColorBitsPerPixel
{
	use_depth,
	use_8, use_16, use_32
};


/** Build a simple LDR frame buffer.
 * 
 * This is mostly used for post-processing.
 * 
 * \ingroup create-framebuffer
 * 
 * @param size The size of the framebuffer.
 * @return The created \ref FrameBuffer.
 */
std::shared_ptr<FrameBuffer> build_simple_framebuffer(DEBUG_LABEL_ARG_MANY const Size& size);


/** Builds a multisample anti-aliasing (MSAA) framebuffer for high-quality rendering.
 *
 * This function creates a framebuffer object (FBO) with multisample support, allowing for smoother edges and reduced aliasing artifacts in rendered images.
 * 
 * \ingroup create-framebuffer
 *
 * @param size The dimensions (width, height) of the framebuffer in pixels.
 * @param msaa_samples The number of MSAA samples to use. Higher values provide better anti-aliasing but may impact performance. Typical values are 2, 4, or 8.
 * @param bits_per_pixel The color buffer precision.
 * @return The created \ref FrameBuffer object with MSAA enabled.
 */
std::shared_ptr<FrameBuffer> build_msaa_framebuffer(DEBUG_LABEL_ARG_MANY const Size& size, int msaa_samples, ColorBitsPerPixel bits_per_pixel);


/** Build a HDR frame buffer.
 * 
 * \ingroup create-framebuffer
 * 
 * @param size The resolution in pixels.
 * @param bits_per_pixel How many bits per pixel to use
 * @return The created \ref FrameBuffer
 */
std::shared_ptr<FrameBuffer> build_hdr_floating_framebuffer(DEBUG_LABEL_ARG_MANY const Size& size, ColorBitsPerPixel bits_per_pixel);


/** Create a depth-only framebuffer for shadow rendering.
 * 
 * \ingroup create-framebuffer
 * 
 * @param size The resolution in pixels.
 * @return The created \ref FrameBuffer
 */
std::shared_ptr<FrameBuffer> build_shadow_framebuffer(DEBUG_LABEL_ARG_MANY const Size& size);


/// raii class to render to a FrameBuffer
struct BoundFbo
{
	std::shared_ptr<FrameBuffer> fbo;

	BoundFbo(const BoundFbo&) = delete;
	BoundFbo(BoundFbo&&) = delete;
	void operator=(const BoundFbo&) = delete;
	void operator=(BoundFbo&&) = delete;

	explicit BoundFbo(std::shared_ptr<FrameBuffer> f);
	~BoundFbo();
};

void resolve_multisampled_buffer(const FrameBuffer& src, FrameBuffer* dst);

/**
 * @}
*/

}
