#pragma once

#include <memory>
#include <vector>

#include "render/instance.h"
#include "render/light.h"

namespace euphoria::core
{
    struct camera3;
    struct compiled_camera3;
}

namespace euphoria::core::vfs
{
    struct file_system;
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
        render(const viewport& viewport, const core::camera3& camera);

        void
        step();

        void
        render
        (
            const core::camera3& camera,
            const core::compiled_camera3& compiled
        );

        std::vector<std::shared_ptr<instance>> actors;
    };
}
