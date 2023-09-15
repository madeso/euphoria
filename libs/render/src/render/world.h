#pragma once

#include <memory>


#include "render/instance.h"
#include "render/light.h"

namespace eu::core
{
    struct Camera3;
    struct CompiledCamera3;
    struct Viewport;
}

namespace eu::core::vfs
{
    struct FileSystem;
}

namespace eu::render
{
    struct MaterialShader;
    struct ShaderUniform;
}

namespace eu::render
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
