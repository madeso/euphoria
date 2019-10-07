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
    class Actor
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

        void
        BeginMaterialOverride(unsigned int index);

        bool
        IsMaterialOverridden(unsigned int index) const;

        CompiledMeshMaterial*
        GetOverriddenMaterial(unsigned int index);

        void
        EndMaterialOverride(unsigned int index);

        core::mat4f
        GetModelMatrix() const;

        void
        Render(const core::mat4f& projection_matrix,
               const core::mat4f& view_matrix,
               const core::vec3f& camera,
               const Light&       light);

        void
        BasicRender(
                const core::mat4f&              projection_matrix,
                const core::mat4f&              view_matrix,
                std::shared_ptr<MaterialShader> shader);


        std::shared_ptr<CompiledMesh> mesh_;
        core::vec3f                   position_;
        core::quatf                   rotation_;

    private:
        std::vector<std::shared_ptr<CompiledMeshMaterial>>
                overridden_materials_;

    public:
        bool      has_outline;
        core::Rgb outline_color;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_ACTOR_H
