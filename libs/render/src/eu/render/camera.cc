#include "eu/render/camera.h"

#include <cmath>

namespace eu::render
{

// todo(Gustav): this bit is weird, build a matrix directly and extract the in vector from it

m4 create_view_from_world_mat(const v3&, const CameraVectors& cv)
{
	return m4::from(Q::look_in_direction(cv.front, kk::up)).value_or(m4_identity);
}

CompiledCamera compile(const Camera& cam, const Size& window_size)
{
	// todo(Gustav): use the near and far from the camera struct
	const float aspect = static_cast<float>(window_size.width) / static_cast<float>(window_size.height);
	const auto clip_from_view = m4::create_perspective(cam.fov, aspect, cam.near, cam.far);

	const auto camera_space = create_vectors(cam.rotation);
	const auto view_from_world = create_view_from_world_mat(cam.position, camera_space);

	return CompiledCamera{
	    .clip_from_view = clip_from_view,
        .view_from_world = view_from_world,
        .position = cam.position,
        .in = camera_space.front
	};
}

CompiledCamera compile(const OrthoCamera& cam, const Size& window_size)
{
	const float aspect = static_cast<float>(window_size.width) / static_cast<float>(window_size.height);

	const float width = cam.size * aspect;
	const float height = cam.size;

	const auto clip_from_view = m4::create_ortho_lrud(-width/2.0f, width/2.0f, height/2.0f, -height/2.0f, cam.near, cam.far);

	const auto camera_space = create_vectors(cam.rotation);
	const auto view_from_world = create_view_from_world_mat(cam.position, camera_space);

	return CompiledCamera{
	    .clip_from_view = clip_from_view,
        .view_from_world = view_from_world,
        .position = cam.position,
        .in = camera_space.front
	};
}

/** Convert from -1..1 to 0..1
 */
float r01_from_r11(float f)
{
	return (f + 1.0f) / 2.0f;
}

v2 screen_from_clip(const v2& resolution, const v4& clip_pos)
{
	const auto ndc = clip_pos.to_vec3_persp_div();
	return v2{r01_from_r11(ndc.x), r01_from_r11(ndc.y)} * resolution;
}

v2 screen_from_world(const CompiledCamera& cam, const v3& world_pos, const v2& resolution)
{
	const auto clip_pos = cam.clip_from_view * cam.view_from_world * v4(world_pos, 1.0f);
	return screen_from_clip(resolution, clip_pos);
}

}  //  namespace eu::render
