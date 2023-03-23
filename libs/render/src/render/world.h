#pragma once

#include <memory>


#include "render/instance.h"
#include "render/light.h"

namespace euphoria::core
{
    struct Camera3;
    struct CompiledCamera3;
    struct Viewport;
}

namespace euphoria::core::vfs
{
    struct FileSystem;
}

namespace euphoria::render
{
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
        render(const core::Viewport& viewport, const core::Camera3& camera);

        void
        update();

        void
        render
        (
            const core::Camera3& camera,
            const core::CompiledCamera3& compiled
        );

        std::vector<std::shared_ptr<Instance>> actors;
    };
}
