#include "core/ui.h"


namespace eu::core
{


v2 calculate_region(const v2& mouse_pos, const v2& pos, const v2& my_tex, const v2& widget_size, float region_sz)
{
	float region_x = (mouse_pos.x - pos.x) * (my_tex.x / widget_size.x) - region_sz * 0.5f;
	float region_y = (mouse_pos.y - pos.y) * (my_tex.y / widget_size.y) - region_sz * 0.5f;
	if (region_x < 0.0f)
	{
		region_x = 0.0f;
	}
	else if (region_x > my_tex.x - region_sz)
	{
		region_x = my_tex.x - region_sz;
	}
	if (region_y < 0.0f)
	{
		region_y = 0.0f;
	}
	else if (region_y > my_tex.y - region_sz)
	{
		region_y = my_tex.y - region_sz;
	}

	return {region_x, region_y};
}



}

