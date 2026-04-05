#pragma once

#include "render/camera.h"

namespace eu::render
{

struct World;
struct DirectionalLight;
struct RenderSettings;

CompiledCamera compile_the_shadow_camera(
	const Camera& camera,
	const Size& window_size,
	const DirectionalLight& light,
	const RenderSettings& settings,
	const World& world
);
    
}
