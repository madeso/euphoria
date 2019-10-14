#include "window/imguilibrary.h"

#include "imgui/imgui.h"
#include "imgui_impl_sdl_gl3.h"

#include "IconsForkAwesome.h"
#include "font_forkawesome.h"

#include "font_noto_sans_display.h"

namespace euphoria::window
{
    void
    AddForkAwesome()
    {
        ImGuiIO& io = ImGui::GetIO();
        
        // io.Fonts->AddFontDefault();
        io.Fonts->AddFontFromMemoryCompressedTTF(
            NotoSansDisplay_compressed_data, NotoSansDisplay_compressed_size, 16);

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


    ImguiLibrary::ImguiLibrary(SDL_Window* window, const std::string& path)
        : window_(window), path_(path + "imgui.ini")
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplSdlGL3_Init(window);

        AddForkAwesome();

        ImGui::GetIO().IniFilename = path_.c_str();
    }

    ImguiLibrary::~ImguiLibrary()
    {
        ImGui_ImplSdlGL3_Shutdown();
        ImGui::DestroyContext();
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
        ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    }

}  // namespace euphoria::window
