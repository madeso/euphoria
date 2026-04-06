#pragma once

#include "eu/render/space.h"

namespace eu::render
{

/** \addtogroup render
 *  @{
*/

// todo(Gustav): rename to PerspectiveCamera? or PerspCamera?
/// The current perspective camera representation.
struct Camera
{
	An fov = 45.0_deg;
	float near = 0.1f;
	float far = 100.0f;

	v3 position = v3{0.0f, 0.0f, 0.0f};

    Ypr rotation = Ypr{ 0.0_rad, 0.0_rad, 0.0_rad };
};

/// A orthographic camera representation.
struct OrthoCamera
{
	float size = 100.0f;

	float near = 0.1f;
	float far = 100.0f;

	v3 position = v3{0.0f, 0.0f, 0.0f};

    Ypr rotation = Ypr{ 0.0_rad, 0.0_rad, 0.0_rad };
};

/// A "compiled" camera for use in rendering
/// @see compile_camera
struct CompiledCamera
{
	/// also known as projection matrix
	m4 clip_from_view;

	/// also known as view matrix
	m4 view_from_world;
	v3 position;
	n3 in;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions

/// Capture the current local space of the camera.
CameraVectors create_vectors(const Camera& camera);

// todo(Gustav): the meaning of this function is unclear... move closer to usage or remove?
m4 create_view_from_world_mat(const v3& pos, const CameraVectors& cv);

/// "Compile" a camera to a perspective so it can be used in rendering.
CompiledCamera compile(const Camera&, const Size& window_size);

/// "Compile" a camera to an orthographic view so it can be used in rendering.
CompiledCamera compile(const OrthoCamera&, const Size& window_size);

// todo(Gustav): also include depth, what happens if the position is behind?
/// Calculate the screen coordinate of a 3d world position.
v2 screen_from_world(const CompiledCamera& cam, const v3& world_pos, const v2& resolution);

/**
@}
*/
}  //  namespace eu::render
