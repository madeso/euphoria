#ifndef EUPHORIA_ACTOR_H
#define EUPHORIA_ACTOR_H

#include <memory>

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/rgb.h"

#include "render/instance.h"
#include "render/compiledmesh.h"

namespace euphoria::render
{
    struct MaterialOverride
    {
        std::vector<CompiledMeshMaterial> materials;
    };
    
    struct Actor : public Instance
    {
    public:
        Actor(const std::shared_ptr<CompiledMesh>& mesh);

        void
        Render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light&       light
        ) override;


        std::shared_ptr<MaterialOverride>
        CreateOverride();
        

        std::shared_ptr<CompiledMesh> mesh_;
        std::shared_ptr<MaterialOverride> overriden_materials;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_ACTOR_H
