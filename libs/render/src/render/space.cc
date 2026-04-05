#include "render/space.h"

namespace eu::render
{


CameraVectors create_vectors(const Ypr& rotation)
{
    const Q rot = Q::from(rotation);

	return {.front = rot.get_local_in(), .right = rot.get_local_right(), .up = rot.get_local_up()};
}


}
