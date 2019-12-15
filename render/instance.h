#ifndef EUPHORIA_RENDER_INSTANCE_H
#define EUPHORIA_RENDER_INSTANCE_H

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"

namespace euphoria::render
{
    struct Light;

    struct Instance
    {
        Instance();


        virtual ~Instance() = default;


        // todo(Gustav): make poisition and rotation public and skip
        // getters and setters or provide a position/rotation dirty flag
        // that the world can clear when it has repositioned the instance
        // in the current layout.


        const core::vec3f&
        GetPosition();


        const core::quatf&
        GetRotation();


        void
        SetPosition(const core::vec3f& position);


        void
        SetRotation(const core::quatf& rotation);


        core::mat4f
        GetModelMatrix() const;


        virtual void
        Render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light&       light
        ) = 0;


    private:
        core::vec3f                   position;
        core::quatf                   rotation;
    };
}

#endif  // EUPHORIA_RENDER_INSTANCE_H
