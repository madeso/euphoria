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
    struct viewport;
    struct material_shader;
    struct shader_uniform;
}

namespace euphoria::render
{
    struct world
    {
        void
        add_actor(const std::shared_ptr<instance>& actor);

        // todo(Gustav): improve light support
        render::light light;

        void
        render(const viewport& viewport, const core::Camera3& camera);

        void
        step();

        void
        render
        (
            const core::Camera3& camera,
            const core::CompiledCamera3& compiled
        );

        std::vector<std::shared_ptr<instance>> actors;
    };
}
