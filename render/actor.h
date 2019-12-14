#ifndef EUPHORIA_ACTOR_H
#define EUPHORIA_ACTOR_H

#include <memory>

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/rgb.h"
#include "render/compiledmesh.h"

namespace euphoria::render
{
    struct MaterialOverride
    {
        std::vector<CompiledMeshMaterial> materials;
    };
    
    struct Actor
    {
    public:
        Actor(const std::shared_ptr<CompiledMesh>& mesh);

        // todo: make poisition and rotation public and skip getters and setters

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

        void
        Render(const core::mat4f& projection_matrix,
               const core::mat4f& view_matrix,
               const core::vec3f& camera,
               const Light&       light);


        std::shared_ptr<MaterialOverride>
        CreateOverride();


        std::shared_ptr<CompiledMesh> mesh_;
        std::shared_ptr<MaterialOverride> overriden_materials;
        core::vec3f                   position_;
        core::quatf                   rotation_;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_ACTOR_H
