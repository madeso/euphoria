#pragma once

#include "dear_imgui/imgui.h"

#include "eu/imgui/scurve.h"

namespace eu::render
{
struct FrameBuffer;
}

namespace eu::imgui
{

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
bool simple_gamma_slider(const char* label, float* gamma, float curve = 1.5f, float min_gamma = 1.6f, float max_gamma = 2.8f);

bool drag(const char* label, v3* drag);
bool drag(const char* label, Ypr* drag);

bool gear(const char* label, v3* drag);
bool gear(const char* label, Rui* drag);
bool gear(const char* label, Ypr* drag);

void transform(const m4& m);
bool transform(m4* m);

struct PropTable
{
    PropTable(const PropTable&) = delete;
    void operator=(const PropTable&) = delete;
    void operator=(const PropTable&&) = delete;

    PropTable(PropTable&& r) noexcept;

    constexpr operator bool() const { return started; }

    static PropTable begin(const char* label = "properties");
    ~PropTable();
    
    bool check(const char* label, bool* v) const;
    bool slider(const char* label, float* v, float min, float max) const;
    bool slider(const char* label, float* v) const;
    void text(const char* label, const std::string& str) const;
    bool edit(const char* label, std::string* str) const;
    void readonly(const char* label, const std::string& text) const;
    void readonly(const char* label, const eu::v3& v) const;
    void readonly(const char* label, const eu::Ypr& y) const;
    bool simple_gamma_slider(const char* label, float* gamma, float curve = 1.5f, float min_gamma = 1.6f, float max_gamma = 2.8f) const;
    bool drag(const char* label, eu::v3* drag) const;
    bool drag(const char* label, eu::Ypr* drag) const;
    bool gear(const char* label, eu::v3* drag) const;
    bool gear(const char* label, eu::Rui* drag) const;
    bool gear(const char* label, eu::Ypr* drag) const;
    void transform(const eu::m4& m) const;
    bool transform(eu::m4* m) const;
    bool scurve(const char* label, eu::core::SCurve* scurve, eu::imgui::SCurveGuiState* gui, eu::imgui::FlipX flip_x, const eu::imgui::SCurveImguiSettings& settings, bool force_init_curve) const;

private:
    bool started;

    PropTable() = default;
    explicit PropTable(bool b);
};


}
