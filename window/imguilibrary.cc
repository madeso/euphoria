#include "window/imguilibrary.h"

#include "imgui/imgui.h"
#include "imgui_impl_sdl_gl3.h"

ImguiLibrary::ImguiLibrary(SDL_Window* window, const std::string& path)
    : window_(window)
    , path_(path + "imgui.ini")
{
  ImGui_ImplSdlGL3_Init(window);
  ImGui::GetIO().IniFilename = path_.c_str();
}

ImguiLibrary::~ImguiLibrary()
{
  ImGui_ImplSdlGL3_Shutdown();
}

void
ImguiLibrary::ProcessEvents(SDL_Event* event)
{
  ImGui_ImplSdlGL3_ProcessEvent(event);
}

void
ImguiLibrary::StartNewFrame()
{
  ImGui_ImplSdlGL3_NewFrame(window_);
}

void
ImguiLibrary::Render()
{
  // ImGui::ShowTestWindow();
  ImGui::Render();
}
