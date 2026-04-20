#pragma once

#include "eu/core/uniform_buffer.h"
#include "eu/render/opengl_labels.h"

namespace eu::render
{

/** \addtogroup render Renderer
 *  @{
*/

struct UniformBuffer
{
	DEBUG_LABEL_EXPLICIT_MANY UniformBuffer(DEBUG_LABEL_ARG_MANY const core::UniformBufferSetup& setup);
	~UniformBuffer();

	void set_mat4(const core::CompiledUniformProp& prop, const m4& m);

	UniformBuffer(const UniformBuffer&) = delete;
	void operator=(const UniformBuffer&) = delete;

	UniformBuffer(UniformBuffer&&) noexcept;
	UniformBuffer& operator=(UniformBuffer&&) noexcept;

	// clears the loaded buffer to a invalid buffer
	void unload();

	unsigned int id;
};

struct BoundUniformBuffer
{
	explicit BoundUniformBuffer(UniformBuffer* b);
	~BoundUniformBuffer();

	BoundUniformBuffer(const BoundUniformBuffer&) = delete;
	BoundUniformBuffer(BoundUniformBuffer&&) = delete;
	void operator=(const BoundUniformBuffer&) = delete;
	void operator=(BoundUniformBuffer&&) = delete;

	UniformBuffer* buffer;
};

/**
 * @}
*/

}  //  namespace eu::render
