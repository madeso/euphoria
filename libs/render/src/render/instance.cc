#include "render/instance.h"

namespace euphoria::render
{
    Instance::Instance()
        : position(core::vec3f::zero())
        , rotation(core::quatf::identity())
    {
    }

    core::mat4f
    Instance::calc_model_matrix() const
    {
        return render::calc_model_matrix(position, rotation);
    }

    [[nodiscard]] core::mat4f
    calc_model_matrix
    (
        const core::vec3f& position,
        const core::quatf& rotation
    )
    {
        return core::mat4f::from_translation(position) * rotation.to_mat4();
    }
}
