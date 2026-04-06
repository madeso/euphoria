#pragma once

namespace eu::render
{

constexpr auto UP = v3(0.0f, 1.0f, 0.0f);

/// A temporary view of the local space of a camera.
struct CameraVectors
{
	n3 front;
	n3 right;
	n3 up;
};

/// Capture the local space of a "camera" us ing only the rotation.
CameraVectors create_vectors(const Ypr& rotation);

}
