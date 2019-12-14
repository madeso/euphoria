#ifndef EUPHORIA_WORLD_H
#define EUPHORIA_WORLD_H

#include <memory>
#include <vector>

#include "render/instance.h"
#include "render/light.h"

namespace euphoria::core
{
    struct Camera;
    struct CompiledCamera;
}  // namespace euphoria::core

namespace euphoria::core::vfs
{
    struct FileSystem;
}

namespace euphoria::render
{
    struct Viewport;
    struct MaterialShader;
    struct ShaderUniform;

    struct World
    {
    public:
        void
        AddActor(const std::shared_ptr<Instance>& actor);

        // todo: improve light support
        Light light;

        void
        Render(const Viewport& viewport, const core::Camera& camera);

        void
        Render(const core::Camera&         camera,
               const core::CompiledCamera& compiled);

    private:
        std::vector<std::shared_ptr<Instance>> actors_;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_WORLD_H
