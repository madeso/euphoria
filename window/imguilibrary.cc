#include "window/imguilibrary.h"

#include "imgui/imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "IconsForkAwesome.h"
#include "font_forkawesome.h"

#include "font_noto_sans_display.h"

#include "window/sdlglcontext.h"

namespace euphoria::window
{
    void
    AddForkAwesome()
    {
        ImGuiIO& io = ImGui::GetIO();

        // io.Fonts->AddFontDefault();
        io.Fonts->AddFontFromMemoryCompressedTTF(
                NotoSansDisplay_compressed_data,
                NotoSansDisplay_compressed_size,
                16);

        static const ImWchar icons_ranges[] = {ICON_MIN_FK, ICON_MAX_FK, 0};
        ImFontConfig         icons_config;
        icons_config.MergeMode        = true;
        icons_config.PixelSnapH       = true;
        icons_config.GlyphMinAdvanceX = 13.0f;

        io.Fonts->AddFontFromMemoryCompressedTTF(
                ForkAwesome_compressed_data,
                ForkAwesome_compressed_size,
                13.0f,
                &icons_config,
                icons_ranges);
    }


    ImguiLibrary::ImguiLibrary(SDL_Window* window, SdlGlContext* context, const std::string& path)
        : window_(window), path_(path + "imgui.ini")
    {
        // hrm... euphoria is using #version 330 core
        // todo(Gustav): look into this...
#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
#endif
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplSDL2_InitForOpenGL(window, context->context);
        ImGui_ImplOpenGL3_Init(glsl_version);

        AddForkAwesome();

        ImGui::GetIO().IniFilename = path_.c_str();
    }

    ImguiLibrary::~ImguiLibrary()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void
    ImguiLibrary::ProcessEvents(SDL_Event* event)
    {
        ImGui_ImplSDL2_ProcessEvent(event);
    }

    void
    ImguiLibrary::StartNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window_);
        ImGui::NewFrame();
    }

    void
    ImguiLibrary::Render()
    {
        // ImGui::ShowTestWindow();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

}  // namespace euphoria::window
