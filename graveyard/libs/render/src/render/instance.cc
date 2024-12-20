#include "render/instance.h"

namespace eu::render
{
    Instance::Instance()
        : position(zero3f)
        , rotation(q_identity)
    {
    }

    mat4f
    Instance::calc_model_matrix() const
    {
        return render::calc_model_matrix(position, rotation);
    }

    [[nodiscard]] mat4f
    calc_model_matrix
    (
        const vec3f& position,
        const quatf& rotation
    )
    {
        return mat4f::from_translation(position) * rotation.to_mat4();
    }
}
