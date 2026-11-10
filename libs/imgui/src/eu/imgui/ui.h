#pragma once

#include "dear_imgui/imgui.h"

namespace eu::render
{
struct FrameBuffer;
}


namespace eu::core
{
    struct SCurve;
}

namespace eu::imgui
{

    /// Represents the GUI state for an S-curve.
    struct SCurveGuiState
    {
        ImVec2 drag = { 0.5f, 0.5f };	 ///< the gui data

        SCurveGuiState() = default;
        SCurveGuiState(float x, float y);

        static constexpr std::size_t num_points = 21;  ///< number of points to draw, more points means smoother curve, but more expensive
        std::array<ImVec2, num_points> point_cache;
    };

    enum class FlipX
    {
        no, yes
    };

    bool imgui_s_curve_editor(const char* title, core::SCurve* scurve, SCurveGuiState* gui, FlipX flip_x, const ImVec2& requested_widget_size = ImVec2{ 100, 100 });


struct ImguiShaderProgram
{
	unsigned int program_handle = 0;
	int texture_attrib = 0;
	int projection_attrib = 0;
};

struct ImguiShaderCache
{
	ImguiShaderCache();
	~ImguiShaderCache();

	ImguiShaderCache(const ImguiShaderCache&) = delete;
	ImguiShaderCache(ImguiShaderCache&&) = delete;
	ImguiShaderCache& operator=(const ImguiShaderCache&) = delete;
	ImguiShaderCache& operator=(ImguiShaderCache&&) = delete;

	ImguiShaderProgram linear_to_gamma_shader;
	ImguiShaderProgram depth_ortho_shader;
};

enum class ImageShader
{
	none,
	tonemap_and_gamma,
	depth_ortho
};

void setup_colors();

void label(const char* label);
bool button(const char* label);
bool centered_button(const char* label);

bool header(const char* label, bool* closeable = nullptr);

void imgui_text(const std::string& str);

bool imgui_edit(const char* label, std::string* str);

void imgui_readonly(const char* label, const std::string& text);
void imgui_readonly(const char* label, const v3& v);
void imgui_readonly(const char* label, const Ypr& y);

void imgui_image(const char* name, const render::FrameBuffer& img, ImguiShaderCache* cache, ImageShader shader);


struct PropTable
{
    PropTable(const PropTable&) = delete;
    void operator=(const PropTable&) = delete;
    void operator=(const PropTable&&) = delete;

    PropTable(PropTable&& r) noexcept;

    constexpr operator bool() const { return started; }

    static PropTable Begin(const char* label = "properties");
    ~PropTable();
    
    bool check(const char* label, bool* v) const;
    bool slider(const char* label, float* v, float min, float max) const;
    bool slider(const char* label, float* v) const;
    void text(const char* label, const std::string& str) const;
    bool edit(const char* label, std::string* str) const;

    void display(const char* label, bool b) const;
    void readonly(const char* label, const std::string& text) const;
    void readonly(const char* label, const eu::v3& v) const;
    void readonly(const char* label, const eu::Ypr& y) const;
    bool color(const char* label, Rgb* color) const;
    bool simple_gamma_slider(const char* label, float* gamma, float curve = 1.5f, float min_gamma = 1.6f, float max_gamma = 2.8f) const;
    bool drag(const char* label, eu::v3* drag, float reset_value = 0.0f) const;
    bool drag(const char* label, eu::Ypr* drag) const;
    bool drag(const char* label, eu::Rui* drag) const;
    bool drag(const char* label, float* drag, float speed = 1.0f) const;
    bool drag(const char* label, int* drag) const;
    void transform(const eu::m4& m) const;
    bool transform(eu::m4* m) const;
    bool scurve(const char* label, eu::core::SCurve* scurve, eu::imgui::SCurveGuiState* gui, eu::imgui::FlipX flip_x, const ImVec2& requested_widget_size = ImVec2{ 100, 100 }) const;

    void image(const char* label, const render::FrameBuffer& img, ImguiShaderCache* cache, ImageShader shader) const;
private:
    bool started;

    PropTable() = default;
    explicit PropTable(bool b);
};


}
