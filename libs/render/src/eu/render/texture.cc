#include "eu/render/texture.h"

#include "eu/assert/assert.h"

#include "eu/log/log.h"

#include "eu/base/ints.h"
#include "eu/base/cint.h"
// #include "eu/base/str.h"
// #include "eu/base/cpp.h"
#include "eu/base/vec4.h"

#include "eu/render/opengl_utils.h"
#include "eu/render/texture.io.h"

#include "dependency_glad.h"

#include "eustb_image.h"

#include <ranges>
#include <algorithm>
#include <array>


namespace eu::render
{


// ------------------------------------------------------------------------------------------------
// general

namespace
{
	/// the color of an image that failed to load, html hot-pink
	constexpr SingleColor image_load_failure_color = color_from_rgba(0xFF, 0x69, 0xB4, 0xFF);

	constexpr unsigned int invalid_id = 0;

	[[nodiscard]]
	unsigned int create_texture()
	{
		unsigned int texture = 0;
		glGenTextures(1, &texture);
		return texture;
	}

	void set_texture_wrap(GLenum target, TextureEdge te, const std::optional<v4>& border_color)
	{
		const auto wrap = te == TextureEdge::clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
		if (border_color.has_value())
		{
			ASSERT(te == TextureEdge::clamp);
			glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border_color->get_data_ptr());
			glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			return;
		}
		glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
	}

	struct MinMagFilter
	{
		GLint min;
		GLint mag;
	};

	[[nodiscard]]
	MinMagFilter min_mag_from_trs(TextureRenderStyle trs)
	{
		switch (trs)
		{
		case TextureRenderStyle::pixel: return {GL_NEAREST, GL_NEAREST};
		case TextureRenderStyle::linear: return {GL_LINEAR, GL_LINEAR};
		case TextureRenderStyle::mipmap: return {GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR};
		default: DIE("Invalid texture render style"); return {GL_NEAREST, GL_NEAREST};
		}
	}

	[[nodiscard]]
	GLint internal_format_from_color_data(Transparency t, ColorData cd)
	{
		const auto include_transparency = t == Transparency::include;
		switch(cd)
		{
		case ColorData::color_data: return include_transparency ? GL_SRGB_ALPHA : GL_SRGB;
		case ColorData::dont_care:
		case ColorData::non_color_data: return include_transparency ? GL_RGBA : GL_RGB;
		default:
			DIE("Invalid color data type");
			return GL_RGBA;
		}
	}
}  //  namespace

// ------------------------------------------------------------------------------------------------
// base texture

BaseTexture::BaseTexture()
	: id(create_texture())
{
}

BaseTexture::~BaseTexture()
{
	unload();
}

BaseTexture::BaseTexture(BaseTexture&& rhs) noexcept
	: id(rhs.id)
{
	rhs.id = invalid_id;
}

BaseTexture& BaseTexture::operator=(BaseTexture&& rhs) noexcept
{
	unload();

	id = rhs.id;

	rhs.id = invalid_id;

	return *this;
}

void BaseTexture::unload()
{
	if (id != invalid_id)
	{
		glDeleteTextures(1, &id);
		id = invalid_id;
	}
}

// ------------------------------------------------------------------------------------------------
// texture 2d

Texture2d::Texture2d(DEBUG_LABEL_ARG_MANY const void* pixel_data, unsigned int pixel_format, int width, int height, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd)
{
	// todo(Gustav): use states
	glBindTexture(GL_TEXTURE_2D, id);
    SET_DEBUG_LABEL_NAMED(id, DebugLabelFor::Texture, fmt::format("TEXTURE2d {}", debug_label));

	set_texture_wrap(GL_TEXTURE_2D, te, std::nullopt);

	const auto filter = min_mag_from_trs(trs);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter.min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter.mag);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		internal_format_from_color_data(t, cd),
		width,
		height,
		0,
		pixel_format,
		GL_UNSIGNED_BYTE,
		pixel_data
	);

	if (trs == TextureRenderStyle::mipmap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}

struct PixelData
{
	stbi_uc* pixel_data = nullptr;
	int width = 0;
	int height = 0;

	PixelData(const embedded_binary& image_binary, bool include_transparency, bool flip = true)
	{
		int junk_channels = 0;
		stbi_set_flip_vertically_on_load(flip ? 1 : 0);

		pixel_data = stbi_load_from_memory(
			reinterpret_cast<const unsigned char*>(image_binary.data),
			int_from_unsigned_int(image_binary.size),
			&width,
			&height,
			&junk_channels,
			include_transparency ? 4 : 3
		);

		if (pixel_data == nullptr)
		{
			LOG_ERR("ERROR: Failed to read pixel data");
			width = 0;
			height = 0;
		}
	}

	~PixelData()
	{
		if (pixel_data != nullptr)
		{
			stbi_image_free(pixel_data);
		}
	}

	PixelData(const PixelData&) = delete;
	PixelData(PixelData&&) = delete;
	void operator=(const PixelData&) = delete;
	void operator=(PixelData&&) = delete;
};

[[nodiscard]]
Texture2d load_image_from_embedded(
	DEBUG_LABEL_ARG_MANY const embedded_binary& image_binary, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd
)
{
	const auto include_transparency = t == Transparency::include;

	auto parsed = PixelData{image_binary, include_transparency};
	if (parsed.pixel_data == nullptr)
	{
		LOG_ERR("ERROR: Failed to load image from image source");
		return load_image_from_color(SEND_DEBUG_LABEL_MANY(debug_label) image_load_failure_color, te, trs, t, cd);
	}

	const GLenum pixel_format = include_transparency ? GL_RGBA : GL_RGB;
	return {SEND_DEBUG_LABEL_MANY(debug_label) parsed.pixel_data, pixel_format, parsed.width, parsed.height, te, trs, t, cd};
}

[[nodiscard]]
Texture2d load_image_from_color(DEBUG_LABEL_ARG_MANY SingleColor pixel, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd)
{
	return {SEND_DEBUG_LABEL_MANY(debug_label) & pixel, GL_RGBA, 1, 1, te, trs, t, cd};
}

// ------------------------------------------------------------------------------------------------
// cubemap

TextureCubemap::TextureCubemap(DEBUG_LABEL_ARG_MANY const std::array<void*, cubemap_size>& pixel_data, int width, int height, ColorData cd)
{
	// todo(Gustav): use states
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	SET_DEBUG_LABEL_NAMED(id, DebugLabelFor::Texture, fmt::format("TEXTURE CUBEMAP {}", debug_label));

	for (size_t index = 0; index < cubemap_size; index += 1)
	{
		glTexImage2D(
			static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index),
			0,
			internal_format_from_color_data(Transparency::exclude, cd),
			width,
			height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			pixel_data[index]
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

[[nodiscard]]
TextureCubemap load_cubemap_from_color(DEBUG_LABEL_ARG_MANY SingleColor pixel, ColorData cd)
{
	return {SEND_DEBUG_LABEL_MANY(debug_label) {&pixel, &pixel, &pixel, &pixel, &pixel, &pixel}, 1, 1, cd};
}

[[nodiscard]]
TextureCubemap load_cubemap_from_embedded(
	DEBUG_LABEL_ARG_MANY
	const std::array<embedded_binary, cubemap_size>& images,
	ColorData cd
)
{
	constexpr auto include_transparency = false;
	constexpr auto flip = false;

	const auto parsed
		= images
		| std::views::transform([](const embedded_binary& img) { return PixelData{img, include_transparency, flip}; });

	const auto image_width = parsed[0].width;
	const auto image_height = parsed[0].height;

	// is loaded ok
	if (std::ranges::any_of(
			parsed  | std::views::transform([](const auto& x) { return x.pixel_data;}),
			[](auto* ptr) { return ptr == nullptr; }
		))
	{
		LOG_ERR("ERROR: Failed to load some cubemap from image source");
		return load_cubemap_from_color(SEND_DEBUG_LABEL_MANY(debug_label) image_load_failure_color, cd);
	}

	if (std::ranges::all_of(
			parsed | std::views::transform([](const auto& x) { return x.width; }),
			[=](const int w) { return w == image_width; }
		))
	{
		// width ok
	}
	else
	{
		LOG_ERR("ERROR: cubemap has inconsistent width");
		return load_cubemap_from_color(SEND_DEBUG_LABEL_MANY(debug_label) image_load_failure_color, cd);
	}

	if (std::ranges::all_of(
			parsed | std::views::transform([](const auto& x) { return x.height; }),
			[=](const int h) { return h == image_height; }
		))
	{
		// height ok
	}
	else
	{
		LOG_ERR("ERROR: cubemap has inconsistent height");
		return load_cubemap_from_color(SEND_DEBUG_LABEL_MANY(debug_label) image_load_failure_color, cd);
	}

	// ok
	return {
		SEND_DEBUG_LABEL_MANY(debug_label)
		{
			parsed[0].pixel_data,
			parsed[1].pixel_data,
			parsed[2].pixel_data,
			parsed[3].pixel_data,
			parsed[4].pixel_data,
			parsed[5].pixel_data
		},
		image_width,
		image_height,
		cd
	};
}


// ------------------------------------------------------------------------------------------------
// framebuffer

[[nodiscard]]
unsigned int create_fbo()
{
	unsigned int fbo = 0;
	glGenFramebuffers(1, &fbo);
	ASSERT(fbo != 0);
	return fbo;
}

FrameBuffer::FrameBuffer(unsigned int f, const Size& s)
	: size(s)
	, fbo(f)
{
}

FrameBuffer::~FrameBuffer()
{
	if (fbo != 0)
	{
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
	}
	if (rbo != 0)
	{
		glDeleteRenderbuffers(1, &rbo);
		rbo = 0;
	}
}

BoundFbo::BoundFbo(std::shared_ptr<FrameBuffer> f)
	: fbo(std::move(f))
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
}

BoundFbo::~BoundFbo()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

[[nodiscard]]
GLenum determine_fbo_internal_format(DepthBits depth, bool add_stencil)
{
	switch (depth)
	{
	case DepthBits::use_16: return add_stencil? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT16;
	case DepthBits::use_24: return add_stencil? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24;
	case DepthBits::use_32: return add_stencil? GL_DEPTH32F_STENCIL8 : GL_DEPTH_COMPONENT32F;
	case DepthBits::use_none: return add_stencil ? GL_STENCIL_INDEX8 : GL_NONE;
	default: ASSERT(false && "invalid enum depth value"); return GL_NONE;
	}
}

// this is templated because opengl can't decide if the internal format is a GLint or GLenum
template<typename R>
R internal_format_from_color_bpp(ColorBitsPerPixel texture_bits, Transparency trans)
{
	const auto include_transparency = trans == Transparency::include;

	switch (texture_bits)
	{
	case ColorBitsPerPixel::use_depth: return GL_DEPTH_COMPONENT;
	case ColorBitsPerPixel::use_8: return include_transparency ? GL_RGBA : GL_RGB;
	case ColorBitsPerPixel::use_16: return include_transparency ? GL_RGBA16F : GL_RGB16F;
	case ColorBitsPerPixel::use_32: return include_transparency ? GL_RGBA32F : GL_RGB32F;
	default:
		DIE("Invalid texture bits value");
		return GL_RGB;
	}
}


/// A builder class for the \ref FrameBuffer
struct FrameBufferBuilder
{
	constexpr explicit FrameBufferBuilder(const Size& s)
		: size(s)
	{
	}

	Size size;

	ColorBitsPerPixel color_bits_per_pixel = ColorBitsPerPixel::use_8;
	DepthBits include_depth = DepthBits::use_none;
	bool include_stencil = false;
	std::optional<v4> border_color = std::nullopt;

	/// 0 samples == no msaa
	int msaa_samples = 0;

	constexpr FrameBufferBuilder& with_msaa(int samples)
	{
		msaa_samples = samples;
		return *this;
	}

	constexpr FrameBufferBuilder& with_depth(DepthBits bits = DepthBits::use_24)
	{
		include_depth = bits;
		return *this;
	}

	constexpr FrameBufferBuilder& with_color_bits(ColorBitsPerPixel bits)
	{
		color_bits_per_pixel = bits;
		return *this;
	}

	constexpr FrameBufferBuilder& with_stencil()
	{
		include_stencil = true;
		return *this;
	}

	constexpr FrameBufferBuilder& with_border_color(const v4& c)
	{
		border_color = c;
		return *this;
	}

	[[nodiscard]]
	std::shared_ptr<FrameBuffer> build(DEBUG_LABEL_ARG_SINGLE) const;
};


std::shared_ptr<FrameBuffer> build_simple_framebuffer(DEBUG_LABEL_ARG_MANY const Size& size)
{
	return FrameBufferBuilder{size}
		.build(USE_DEBUG_LABEL(debug_label));
}


std::shared_ptr<FrameBuffer> build_hdr_floating_framebuffer(DEBUG_LABEL_ARG_MANY const Size& size, ColorBitsPerPixel bits_per_pixel)
{
	return FrameBufferBuilder{size}
		.with_color_bits(bits_per_pixel)
		.build(USE_DEBUG_LABEL(debug_label));
}


std::shared_ptr<FrameBuffer> build_msaa_framebuffer(DEBUG_LABEL_ARG_MANY const Size& size, int msaa_samples, ColorBitsPerPixel bits_per_pixel)
{
	return FrameBufferBuilder{size}
		.with_msaa(msaa_samples)
		.with_color_bits(bits_per_pixel)
		.with_depth()
		.with_stencil()
		.build(USE_DEBUG_LABEL(debug_label));
}


std::shared_ptr<FrameBuffer> build_shadow_framebuffer(DEBUG_LABEL_ARG_MANY const Size& size)
{
	return FrameBufferBuilder{size}
		.with_color_bits(ColorBitsPerPixel::use_depth)
		.with_border_color(v4{1.0f, 1.0f, 1.0f, 1.0f})
		.build(USE_DEBUG_LABEL(debug_label));
}


std::shared_ptr<FrameBuffer> FrameBufferBuilder::build(DEBUG_LABEL_ARG_SINGLE) const
{
	const auto te = TextureEdge::clamp;
	const auto trs = TextureRenderStyle::linear;
	const auto trans = Transparency::exclude;

	const bool is_msaa = msaa_samples > 0;

	LOG_INFO("Creating frame buffer %d %d", size.width, size.height);
	auto fbo = std::make_shared<FrameBuffer>(create_fbo(), size);
	ASSERT(fbo->id > 0);

	fbo->debug_is_msaa = is_msaa;

	// setup texture
	const GLenum target = is_msaa ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	glBindTexture(target, fbo->id);
	SET_DEBUG_LABEL_NAMED(fbo->id, DebugLabelFor::Texture, fmt::format("TEXTURE FRAMEBUFFER {}", debug_label));
	if (is_msaa == false)
	{
		// msaa neither support min/mag filters nor texture wrapping
		set_texture_wrap(target, te, border_color);
		const auto filter = min_mag_from_trs(trs);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter.min);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter.mag);
	}

	if (is_msaa)
	{
		glTexImage2DMultisample(
			target,
			msaa_samples,
			internal_format_from_color_bpp<GLenum>(color_bits_per_pixel, trans),
			size.width,
			size.height,
			GL_TRUE
		);
	}
	else
	{
		glTexImage2D(
			target,
			0,
			internal_format_from_color_bpp<GLint>(color_bits_per_pixel, trans),
			size.width,
			size.height,
			0,
			// todo(Gustav): since we pass null as the data, the type doesn't matter... right?
			color_bits_per_pixel == ColorBitsPerPixel::use_depth ? GL_DEPTH_COMPONENT : GL_RGB,
			color_bits_per_pixel == ColorBitsPerPixel::use_depth ? GL_FLOAT : GL_UNSIGNED_BYTE,
			nullptr
		);
	}

	// setup fbo
	auto bound = BoundFbo{fbo};
	SET_DEBUG_LABEL_NAMED(fbo->fbo, DebugLabelFor::FrameBuffer, fmt::format("FBO {}", debug_label));
	constexpr GLint mipmap_level = 0;
	glFramebufferTexture2D(GL_FRAMEBUFFER, color_bits_per_pixel == ColorBitsPerPixel::use_depth? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, target, fbo->id, mipmap_level);

	if (color_bits_per_pixel == ColorBitsPerPixel::use_depth)
	{
		// disable color buffer when using depth texture _only_
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	const auto internal_format = determine_fbo_internal_format(include_depth, include_stencil);
	if (internal_format != GL_NONE)
	{
		ASSERT(color_bits_per_pixel != ColorBitsPerPixel::use_depth);
		glGenRenderbuffers(1, &fbo->rbo);
		ASSERT(fbo->rbo != 0);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);
		SET_DEBUG_LABEL_NAMED(fbo->rbo, DebugLabelFor::RenderBuffer, fmt::format("TEXTURE RENDBUFF {}", debug_label));

		if (is_msaa)
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa_samples, internal_format, size.width, size.height);
		}
		else
		{
			glRenderbufferStorage(GL_RENDERBUFFER, internal_format, size.width, size.height);
		}

		if (include_stencil)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->rbo);
		}
	}
	else
	{
		fbo->rbo = 0;
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERR("Failed to create frame buffer");
		return nullptr;
	}

	return fbo;
}


void resolve_multisampled_buffer(const FrameBuffer& src, FrameBuffer* dst)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src.fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->fbo);

	ASSERT(src.size == dst->size);

	glBlitFramebuffer(0, 0, src.size.width, src.size.height, 0, 0, dst->size.width, dst->size.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


}
