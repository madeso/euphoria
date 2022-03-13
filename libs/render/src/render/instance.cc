#include "render/instance.h"

namespace euphoria::render
{
    Instance::Instance()
        : position(core::Vec3f::zero())
        , rotation(core::Quatf::identity())
    {
    }

    core::Mat4f
    Instance::calculate_model_matrix() const
    {
        return render::calculate_model_matrix(position, rotation);
    }

    [[nodiscard]] core::Mat4f
    calculate_model_matrix
    (
        const core::Vec3f& position,
        const core::Quatf& rotation
    )
    {
        return core::Mat4f::from_translation(position) * rotation.to_mat4();
    }
}
