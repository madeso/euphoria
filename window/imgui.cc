#include "window/imgui.h"

#include "imgui/imgui.h"
#include "imgui_impl_sdl_gl3.h"

Imgui::Imgui(SDL_Window *window)
  : window_(window)
{
  ImGui_ImplSdlGL3_Init(window);
  ImGui::GetIO().IniFilename = nullptr;
}

Imgui::~Imgui()
{
  ImGui_ImplSdlGL3_Shutdown();
}

void Imgui::ProcessEvents(SDL_Event* event)
{
  ImGui_ImplSdlGL3_ProcessEvent(event);
}

void Imgui::Begin()
{
  ImGui_ImplSdlGL3_NewFrame(window_);
}

void Imgui::Render()
{
  ImGui::Render();
}
