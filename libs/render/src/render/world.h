#pragma once

#include <memory>
#include <vector>

#include "render/instance.h"
#include "render/light.h"

namespace euphoria::core
{
    struct Camera3;
    struct CompiledCamera3;
}

namespace euphoria::core::vfs
{
    struct FileSystem;
}

namespace euphoria::render
{
    struct Viewport;
    struct MaterialShader;
    struct ShaderUniform;
}

namespace euphoria::render
{
    struct World
    {
        void
        add_actor(const std::shared_ptr<Instance>& actor);

        // todo(Gustav): improve light support
        render::Light light;

        core::CompiledCamera3
        render(const Viewport& viewport, const core::Camera3& camera);

        void
        step();

        void
        render
        (
            const core::Camera3& camera,
            const core::CompiledCamera3& compiled
        );

        std::vector<std::shared_ptr<Instance>> actors;
    };
}
