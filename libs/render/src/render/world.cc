#include "render/world.h"

#include "core/assert.h"
#include "core/camera.h"

#include "render/materialshader.h"
#include "render/viewport.h"
#include "render/gl.h"

#include <algorithm>

namespace euphoria::render
{
    void
    World::AddActor(const std::shared_ptr<Instance>& actor)
    {
        actors_.push_back(actor);
    }

    void
    World::Render(const Viewport& viewport, const core::Camera& camera)
    {
        Render(camera, camera.Compile(viewport.GetAspectRatio()));
    }

    void
    World::Step()
    {
        actors_.erase
        (
            std::remove_if
            (
                actors_.begin(),
                actors_.end(),
                [](const std::shared_ptr<Instance>& instance)
                {
                    return instance->remove_this;
                }
            ),
            actors_.end()
        );
    }

    void
    World::Render
    (
        const core::Camera& camera,
        const core::CompiledCamera& compiled
    )
    {
        const auto projection_matrix = compiled.projection;
        const auto view_matrix = compiled.view;

        for(const auto& actor: actors_)
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
            actor->Render(projection_matrix, view_matrix, camera.position, light);
        }
    }

}
