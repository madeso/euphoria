#pragma once

namespace eu::kk
{

// OpenGL should support at least 16 textures
constexpr std::size_t max_textures_supported = 16;

// todo(Gustav): move these to some (render) settings

constexpr float almost_zero = 0.01f;
/// if alpha goes above this limit, it is no longer considered transparent
constexpr float alpha_transparency_limit = 1.0f - almost_zero;

constexpr float outline_scale = 1.1f;

constexpr int blur_samples = 10;

}

