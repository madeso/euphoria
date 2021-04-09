#include "render/instance.h"

namespace euphoria::render
{
    Instance::Instance()
        : position(core::vec3f::Zero())
        , rotation(core::quatf::Identity())
    {}


    const core::vec3f&
    Instance::GetPosition() const
    {
        return position;
    }


    const core::quatf&
    Instance::GetRotation() const
    {
        return rotation;
    }


    void
    Instance::SetPosition(const core::vec3f& new_position)
    {
        position = new_position;
    }


    void
    Instance::SetRotation(const core::quatf& new_rotation)
    {
        rotation = new_rotation;
    }


    core::mat4f
    Instance::GetModelMatrix() const
    {
        return core::mat4f::FromTranslation(position) * rotation.ToMat4();
    }
}
