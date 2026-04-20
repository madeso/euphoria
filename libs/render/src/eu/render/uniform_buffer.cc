#include "eu/render/uniform_buffer.h"

#include "eu/render/opengl_labels.h"
#include "eu/render/opengl_utils.h"

namespace eu::render
{

namespace
{
	const UniformBuffer* bound_buffer = nullptr;
}  //  namespace

BoundUniformBuffer::BoundUniformBuffer(UniformBuffer* b)
	: buffer(b)
{
	ASSERT(bound_buffer == nullptr);
	bound_buffer = b;
	glBindBuffer(GL_UNIFORM_BUFFER, b->id);
}

BoundUniformBuffer::~BoundUniformBuffer()
{
	ASSERT(bound_buffer == buffer);
	bound_buffer = nullptr;
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::UniformBuffer(DEBUG_LABEL_ARG_MANY const core::UniformBufferSetup& setup)
	: id(create_buffer())
{
	auto bound = BoundUniformBuffer{this};
	SET_DEBUG_LABEL_NAMED(id, DebugLabelFor::Buffer, fmt::format("UNI B {}", debug_label));

	// Changed to a dynamic draw due to:
	//	Using glBufferSubData(...) to update a GL_STATIC_DRAW buffer
	//	Performance Severity: medium
	// todo(Gustav): profile? provide a hint in an argument?
	glBufferData(GL_UNIFORM_BUFFER, setup.size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, gluint_from_int(setup.binding_point), id);
}

UniformBuffer::~UniformBuffer()
{
	unload();
}

UniformBuffer::UniformBuffer(UniformBuffer&& rhs) noexcept
	: id(rhs.id)
{
	rhs.id = 0;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& rhs) noexcept
{
	unload();
	id = rhs.id;
	rhs.id = 0;
	return *this;
}

// clears the loaded buffer to a invalid buffer
void UniformBuffer::unload()
{
	if (id == 0)
	{
		return;
	}

	destroy_buffer(id);
	id = 0;
}

void UniformBuffer::set_mat4(const core::CompiledUniformProp& prop, const m4& m) // NOLINT(readability-convert-member-functions-to-static)
{
	// todo(Gustav): verify that the prop belongs to self
    ASSERT(prop.type == core::UniformType::mat4 && prop.array_count == 1);
	glBufferSubData(GL_UNIFORM_BUFFER, prop.offset, 16 * sizeof(float), m.get_column_major_data_ptr());
}

}  //  namespace eu::render
