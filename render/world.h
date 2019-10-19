#ifndef EUPHORIA_WORLD_H
#define EUPHORIA_WORLD_H

#include <memory>
#include <vector>

#include "render/actor.h"
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
        explicit World(core::vfs::FileSystem* file_system);

        void
        AddActor(const std::shared_ptr<Actor>& actor);

        // todo: improve light support
        Light light;

        // todo: move to a better place
        std::shared_ptr<MaterialShader> outline_shader;
        std::shared_ptr<ShaderUniform>  outline_color;

        void
        Render(const Viewport& viewport, const core::Camera& camera);

        void
        Render(const core::Camera&         camera,
               const core::CompiledCamera& compiled);

        bool use_outline = false;

    private:
        std::vector<std::shared_ptr<Actor>> actors_;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_WORLD_H
