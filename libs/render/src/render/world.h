#pragma once

#include <memory>
#include <vector>

#include "render/instance.h"
#include "render/light.h"

namespace euphoria::core
{
    struct Camera;
    struct CompiledCamera;
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
        AddActor(const std::shared_ptr<Instance>& actor);

        // todo(Gustav): improve light support
        Light light;

        void
        Render(const Viewport& viewport, const core::Camera& camera);

        void
        Step();

        void
        Render(const core::Camera& camera,
               const core::CompiledCamera& compiled);

        std::vector<std::shared_ptr<Instance>> actors_;
    };
}
