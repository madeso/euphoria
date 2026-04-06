#include "eu/render/uniform.h"

namespace eu::render
{
Uniform::Uniform(std::string n, int l, unsigned int sp)
	: name(std::move(n))
	  , location(l)
	, debug_shader_program(sp)
{
}

bool Uniform::is_valid() const
{
	return location >= 0;
}

}
