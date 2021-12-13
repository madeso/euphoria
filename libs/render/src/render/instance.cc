#include "render/instance.h"

namespace euphoria::render
{
    Instance::Instance()
        : position(core::Vec3f::zero())
        , rotation(core::Quatf::identity())
    {
    }

    core::mat4f
    Instance::calculate_model_matrix() const
    {
        return core::mat4f::from_translation(position) * rotation.to_mat4();
    }
}
