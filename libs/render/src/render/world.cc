#include "render/world.h"

#include "assert/assert.h"
#include "core/camera3.h"
#include "core/viewport.h"

#include "render/materialshader.h"
#include "render/gl.h"

#include <algorithm>

namespace euphoria::render
{
    void
    World::add_actor(const std::shared_ptr<Instance>& actor)
    {
        actors.push_back(actor);
    }

    core::CompiledCamera3
    World::render(const core::Viewport& viewport, const core::Camera3& camera)
    {
        const auto compiled = camera.compile(viewport.get_aspect_ratio());
        render(camera, compiled);
        return compiled;
    }

    void
    World::step()
    {
        actors.erase
        (
            std::remove_if
            (
                actors.begin(),
                actors.end(),
                [](const std::shared_ptr<Instance>& instance)
                {
                    return instance->remove_this;
                }
            ),
            actors.end()
        );
    }

    void
    World::render
    (
        const core::Camera3& camera,
        const core::CompiledCamera3& compiled
    )
    {
        const auto projection_matrix = compiled.projection;
        const auto view_matrix = compiled.view;

        for(const auto& actor: actors)
        {
            if(actor->remove_this)
            {
                continue;
            }

            // todo(Gustav): instead of direct rendering, move to a material sorting/render
            // command system
            // general design: http://realtimecollisiondetection.net/blog/?p=86
            // distance from camera:
            // https://gamedev.stackexchange.com/questions/56810/how-do-games-deal-with-z-sorting-partially-transparent-foliage-textures
            // distance in integer:
            // http://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
            // useful?
            // https://gamedev.stackexchange.com/questions/45626/how-to-organize-rendering
            actor->render(projection_matrix, view_matrix, camera.position, light);
        }
    }

}
