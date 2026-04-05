#include "imgui_extra/scurve.h"

#include "assert/assert.h"
#include "base/cint.h"

#include "imgui_extra/ui.h"

#include "imgui.h"

#include <cmath>

#include "core/scurve.h"

namespace eu::imgui_extra
{

SCurveGuiState::SCurveGuiState(float x, float y)
	: drag(x, y)
{
}

SCurveGuiState SCurveGuiState::light_curve()
{
	return {0.2f, 0.0f};
}

bool imgui_s_curve_editor(const char* title, core::SCurve* curve, SCurveGuiState* gui, FlipX flip_x, const SCurveImguiSettings& settings, bool force_init_curve)
{
    imgui_text(fmt::format("{} ({} {})", title, curve->slope, curve->threshold));
	if (ImGui::BeginChild(title, settings.widget_size, settings.widget_border ? ImGuiChildFlags_Borders : ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)
		== false)
	{
		ImGui::EndChild();
		return false;
	}

	auto& points = gui->point_cache;

	bool changed = false;

	const auto widget_position = ImGui::GetWindowPos();
	const auto widget_size = ImGui::GetWindowSize();

	auto* draw = ImGui::GetWindowDrawList();
	draw->AddRectFilled(widget_position, widget_position + widget_size, settings.background_color);

	
	if (points.max_size() != settings.num_points)
	{
		points.resize(settings.num_points);
	}
	for (std::size_t point_index = 0; point_index < settings.num_points; point_index += 1)
	{
		const float srcx = static_cast<float>(point_index) / static_cast<float>(settings.num_points - 1); // -1 here to include 1.0f when evaluating the curve
		const float x = flip_x == FlipX::yes? 1 - srcx : srcx;
		const float y = core::calculate_s_curve(srcx, curve->slope, curve->threshold);
		points[point_index] = ImVec2{x * widget_size.x, (1 - y) * widget_size.y} + widget_position;
	}
	draw->AddPolyline(points.data(), int_from_sizet(settings.num_points), settings.line_color, ImDrawFlags_None, 1.0f);

	if (settings.draw_points)
	{
		for (std::size_t point_index = 0; point_index < settings.num_points; point_index += 1)
		{
			draw->AddCircleFilled(points[point_index], settings.point_radius, settings.point_color);
		}
	}
	draw->AddCircleFilled(ImVec2{gui->drag.x * widget_size.x, (1 - gui->drag.y) * widget_size.y} + widget_position, settings.drag_radius, settings.drag_color);

	const auto t01 = [&widget_position, &widget_size](const ImVec2& mp)
	{
		const auto x = (mp.x - widget_position.x) / widget_size.x;
		const auto y = 1 - ((mp.y - widget_position.y) / widget_size.y);
		return ImVec2{x, y};
	};

	const auto within01 = [](const ImVec2& p)
	{
#define T(s) \
	do \
	{ \
		if ((s) < 0.0f || (s) > 1.0f) \
			return false; \
	} while (false)
		T(p.x);
		T(p.y);
		return true;
#undef T
	};

	if (ImGui::IsMouseDown(settings.button) && within01(t01(ImGui::GetIO().MouseClickedPos[settings.button])))
	{
		// mouse is down and drag started inside the component
		const auto mp = ImGui::GetMousePos();
		const auto n = t01(mp);
		gui->drag.x = std::min(1.0f, std::max(n.x, 0.0f));
		gui->drag.y = std::min(1.0f, std::max(n.y, 0.0f));
		changed = true;
	}

	if (changed || force_init_curve)
	{
		*curve = core::s_curve_from_input(flip_x == FlipX::yes ? 1 - gui->drag.x : gui->drag.x, gui->drag.y);
	}

	ImGui::EndChild();

	return changed;
}

}  //  namespace eu::imgui_extra
