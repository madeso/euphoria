#pragma once

namespace eu::render
{

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
