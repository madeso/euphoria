#pragma once

namespace eu::render
{

// OpenGL should support atleast 16 textures
constexpr std::size_t MAX_TEXTURES_SUPPORTED = 16;

// todo(Gustav): move theese to some (render) settings

constexpr float ALMOST_ZERO = 0.01f;
/// if alpha goes above this limit, it is no longer considered transparent
constexpr float ALPHA_TRANSPARENCY_LIMIT = 1.0f - ALMOST_ZERO;

constexpr float OUTLINE_SCALE = 1.1f;

constexpr int BLUR_SAMPLES = 10;

}

