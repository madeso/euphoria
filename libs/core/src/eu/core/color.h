#pragma once

namespace eu::core
{


/// A single color in a format to load directly into open gl texture(ABGR on little endian).
/// @see \ref color_from_rgba
enum class SingleColor : std::uint32_t {};

/// Constructs a \ref SingleColor value from individual red, green, blue, and alpha components.
/// @param r The red component of the color (0x00 - 0xFF).
/// @param g The green component of the color (0x00 - 0xFF).
/// @param b The blue component of the color (0x00 - 0xFF).
/// @param a The alpha (opacity) component of the color (0x00 - 0xFF).
/// @return A \ref SingleColor value representing the color composed of the specified RGBA components.
constexpr SingleColor color_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return static_cast<SingleColor>((static_cast<uint32_t>(a) << 24) |
		   (static_cast<uint32_t>(b) << 16) |
		   (static_cast<uint32_t>(g) << 8)  |
		   (static_cast<uint32_t>(r)));
}


}