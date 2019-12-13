#include "render/actor.h"

#include "core/assert.h"

namespace euphoria::render
{
    Actor::Actor(const std::shared_ptr<CompiledMesh>& mesh)
        : mesh_(mesh)
        , overriden_materials()
        , position_(core::vec3f::Zero())
        , rotation_(core::quatf::Identity())
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

    std::shared_ptr<MaterialOverride>
    Actor::CreateOverride()
    {
        const auto s = mesh_->materials.size();
        auto r = std::make_shared<MaterialOverride>();
        r->materials.resize(s);
        for(std::size_t index=0; index<s; index+=1)
        {
            r->materials[index] = mesh_->materials[index];
        }
        return r;
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
                overriden_materials);
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
