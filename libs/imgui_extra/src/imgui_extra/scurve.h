#pragma once

#include "imgui.h"

namespace eu::core
{
    struct SCurve;
}

namespace eu::imgui_extra
{

/// Represents the GUI state for an S-curve.
struct SCurveGuiState
{
	ImVec2 drag = {0.5f, 0.5f};	 ///< the gui data

	SCurveGuiState() = default;
	SCurveGuiState(float x, float y);

	static SCurveGuiState light_curve();

	std::vector<ImVec2> point_cache;
};

/// Configuration settings for displaying and interacting with an S-curve widget.
struct SCurveImguiSettings
{
	bool widget_border = false;	 ///< draw a border around the widget
	ImVec2 widget_size = ImVec2{100, 100};	///< size of the widget

	float drag_radius = 10.0f;
	ImU32 drag_color = IM_COL32(100, 0, 0, 255);

	float point_radius = 3.0f;
	ImU32 point_color = IM_COL32(0, 100, 0, 255);
	bool draw_points = false;

	ImGuiMouseButton_ button = ImGuiMouseButton_Left;

	ImU32 background_color = IM_COL32(50, 50, 50, 255);
	ImU32 line_color = IM_COL32(100, 100, 100, 255);

	std::size_t num_points = 21;  ///< number of points to draw, more points means smoother curve, but more expensive
};

enum class FlipX
{
	no, yes
};

bool imgui_s_curve_editor(const char* title, core::SCurve* scurve, SCurveGuiState* gui, FlipX flip_x, const SCurveImguiSettings& settings, bool force_init_curve);

/**
 * @}
*/

}  //  namespace klotter
