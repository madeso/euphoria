#include "eu/render/space.h"

namespace eu::render
{


CameraVectors create_vectors(const Ypr& rotation)
{
    const Q rot = Q::From(rotation);

	return {.front = rot.get_local_in(), .right = rot.get_local_right(), .up = rot.get_local_up()};
}


}
