#include "render/actor.h"

#include "core/assert.h"

namespace euphoria::render
{
    Actor::Actor(const std::shared_ptr<CompiledMesh>& mesh)
        : mesh_(mesh)
        , position_(core::vec3f::Zero())
        , rotation_(core::quatf::Identity())
        , overridden_materials_(mesh->GetNoOverriddenMaterials())
        , has_outline(false)
        , outline_color(core::Color::White)
    {
        ASSERT(mesh);
    }

    const core::vec3f&
    Actor::GetPosition()
    {
        return position_;
    }

    const core::quatf&
    Actor::GetRotation()
    {
        return rotation_;
    }

    void
    Actor::SetPosition(const core::vec3f& position)
    {
        position_ = position;
    }

    void
    Actor::SetRotation(const core::quatf& rotation)
    {
        rotation_ = rotation;
    }

    void
    Actor::BeginMaterialOverride(unsigned int index)
    {
        ASSERT(index < overridden_materials_.size());
        ASSERT(!IsMaterialOverridden(index));
        overridden_materials_[index] = std::make_shared<CompiledMeshMaterial>(
                mesh_->materials[index]);
    }

    bool
    Actor::IsMaterialOverridden(unsigned int index) const
    {
        ASSERT(index < overridden_materials_.size());
        return overridden_materials_[index] != nullptr;
    }

    CompiledMeshMaterial*
    Actor::GetOverriddenMaterial(unsigned int index)
    {
        ASSERT(index < overridden_materials_.size());
        ASSERT(IsMaterialOverridden(index));
        return overridden_materials_[index].get();
    }

    void
    Actor::EndMaterialOverride(unsigned int index)
    {
        ASSERT(index < overridden_materials_.size());
        ASSERT(!IsMaterialOverridden(index));
        overridden_materials_[index] = nullptr;
    }

    core::mat4f
    Actor::GetModelMatrix() const
    {
        return core::mat4f::FromTranslation(position_) * rotation_.ToMat4();
    }

    void
    Actor::Render(
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light&       light)
    {
        mesh_->Render(
                GetModelMatrix(),
                projection_matrix,
                view_matrix,
                camera,
                light,
                overridden_materials_);
    }

    void
    Actor::BasicRender(
            const core::mat4f&              projection_matrix,
            const core::mat4f&              view_matrix,
            std::shared_ptr<MaterialShader> shader)
    {
        mesh_->BasicRender(
                GetModelMatrix(), projection_matrix, view_matrix, shader);
    }

}  // namespace euphoria::render
