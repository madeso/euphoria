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
    struct Viewport;
    struct MaterialShader;
    struct ShaderUniform;
}

namespace euphoria::render
{
    struct World
    {
        void
        AddActor(const std::shared_ptr<Instance>& actor);

        // todo(Gustav): improve light support
        Light light;

        void
        Render(const Viewport& viewport, const core::camera3& camera);

        void
        Step();

        void
        Render(const core::camera3& camera,
               const core::compiled_camera3& compiled);

        std::vector<std::shared_ptr<Instance>> actors_;
    };
}
