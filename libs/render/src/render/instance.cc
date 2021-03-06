#include "render/instance.h"

namespace euphoria::render
{
    instance::instance()
        : position(core::vec3f::zero())
        , rotation(core::quatf::identity())
    {
    }

    core::mat4f
    instance::calculate_model_matrix() const
    {
        return core::mat4f::from_translation(position) * rotation.to_mat4();
    }
}
