#include "render/world.h"

#include "core/assert.h"
#include "core/camera3.h"

#include "render/materialshader.h"
#include "render/viewport.h"
#include "render/gl.h"

#include <algorithm>

namespace euphoria::render
{
    void
    world::add_actor(const std::shared_ptr<instance>& actor)
    {
        actors.push_back(actor);
    }

    void
    world::render(const viewport& viewport, const core::Camera3& camera)
    {
        render(camera, camera.compile(viewport.get_aspect_ratio()));
    }

    void
    world::step()
    {
        actors.erase
        (
            std::remove_if
            (
                actors.begin(),
                actors.end(),
                [](const std::shared_ptr<instance>& instance)
                {
                    return instance->remove_this;
                }
            ),
            actors.end()
        );
    }

    void
    world::render
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
