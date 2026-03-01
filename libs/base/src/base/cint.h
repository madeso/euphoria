#pragma once

namespace eu
{

constexpr float float_from_int(int i)
{
	return static_cast<float>(i);
}

constexpr int int_from_float(float f)
{
	return static_cast<int>(f);
}

constexpr std::size_t sizet_from_int(int i)
{
	return static_cast<std::size_t>(i);
}

constexpr u32 u32_from_sizet(std::size_t i)
{
	return static_cast<u32>(i);
}

constexpr int int_from_sizet(std::size_t i)
{
	return static_cast<int>(i);
}

constexpr int int_from_unsigned_int(unsigned int i)
{
	return static_cast<int>(i);
}

}
