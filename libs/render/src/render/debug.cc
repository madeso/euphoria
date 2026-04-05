#include "render/debug.h"

#include "render/camera.h"

namespace eu::render
{

void DebugRender::add_line(const v3& from, const v3& to, const Rgb& color)
{
	lines.emplace_back(DebugLine{from, to, color});
}

v3 world_from_ndc(const CompiledCamera& camera, const v3& ndc)
{
    const auto clip_space_pos = v4{ndc, 1.0f};
	const auto view_from_clip = camera.clip_from_view.get_inverted();
	const auto view_space_pos = view_from_clip * clip_space_pos;
	const auto view_space_pos_corrected = view_space_pos.to_vec3_persp_div();
	const auto world_from_view = camera.view_from_world.get_inverted();
    const auto world_space_pos = world_from_view * v4{ view_space_pos_corrected, 1.0f };
	const auto out_world = v3{world_space_pos.x, world_space_pos.y, world_space_pos.z};
	return out_world;
}

void draw_frustum(DebugRender* debug, const CompiledCamera& camera, const Rgb& color)
{
	const auto near_bot_left = world_from_ndc(camera, {-1.0f, -1.0f, -1.0f});
	const auto near_bot_right = world_from_ndc(camera, {+1.0f, -1.0f, -1.0f});
	const auto near_top_left = world_from_ndc(camera, {-1.0f, +1.0f, -1.0f});
	const auto near_top_right = world_from_ndc(camera, {+1.0f, +1.0f, -1.0f});
	const auto far_bot_left = world_from_ndc(camera, {-1.0f, -1.0f, +1.0f});
	const auto far_bot_right = world_from_ndc(camera, {+1.0f, -1.0f, +1.0f});
	const auto far_top_left = world_from_ndc(camera, {-1.0f, +1.0f, +1.0f});
	const auto far_top_right = world_from_ndc(camera, {+1.0f, +1.0f, +1.0f});

	debug->add_line(camera.position, near_bot_left, color);
	debug->add_line(camera.position, near_top_left, color);
	debug->add_line(camera.position, near_bot_right, color);
	debug->add_line(camera.position, near_top_right, color);
	debug->add_line(near_bot_left, near_bot_right, color);
	debug->add_line(near_bot_right, near_top_right, color);
	debug->add_line(near_top_right, near_top_left, color);
	debug->add_line(near_top_left, near_bot_left, color);
	debug->add_line(far_bot_left, far_bot_right, color);
	debug->add_line(far_bot_right, far_top_right, color);
	debug->add_line(far_top_right, far_top_left, color);
	debug->add_line(far_top_left, far_bot_left, color);
	debug->add_line(near_bot_left, far_bot_left, color);
	debug->add_line(near_bot_right, far_bot_right, color);
	debug->add_line(near_top_left, far_top_left, color);
	debug->add_line(near_top_right, far_top_right, color);
}


}
