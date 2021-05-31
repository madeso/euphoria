#include "window/imguilibrary.h"

#include <cstdint>

#include "imgui/imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "font_material_design_icons.h"

#include "font_noto_sans_display.h"

#include "core/log.h"

#include "window/sdlglcontext.h"
#include "window/imgui_icons.h"


namespace euphoria::window::imgui
{
    void
    add_fork_awesome()
    {
        ImGuiIO& io = ImGui::GetIO();

        // io.Fonts->AddFontDefault();
        io.Fonts->AddFontFromMemoryCompressedTTF
        (
            NotoSansDisplay_compressed_data,
            NotoSansDisplay_compressed_size,
            16
        );

        const uint64_t min_mdi = ICON_MIN_MDI;
        const uint64_t max_mdi = ICON_MAX_MDI;
        const uint64_t min_imgui = static_cast<uint64_t>(std::numeric_limits<ImWchar>::min());
        const uint64_t max_imgui = static_cast<uint64_t>(std::numeric_limits<ImWchar>::max());
        const auto max_arg = static_cast<ImWchar>(std::min(max_mdi, max_imgui));
        if(max_mdi > max_imgui)
        {
            LOG_ERROR("imgui is missing {0} icons!", (max_mdi - max_imgui));

            LOG_INFO("mdi range: {0}", (max_mdi - min_mdi));
            LOG_INFO("imgui range: {0}", (max_imgui - min_imgui));
            LOG_INFO("Supplied range: {0}", (max_arg - min_mdi));
        }


        static const ImWchar icons_ranges[] = {ICON_MIN_MDI, max_arg, 0};
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.GlyphMinAdvanceX = 13.0f;

        io.Fonts->AddFontFromMemoryCompressedTTF
        (
            MaterialDesignIcons_compressed_data,
            MaterialDesignIcons_compressed_size,
            13.0f,
            &icons_config,
            icons_ranges
        );
    }


    library::library(SDL_Window* window, sdl_gl_context* context, const std::string& path)
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

        add_fork_awesome();

        ImGui::GetIO().IniFilename = path_.c_str();
    }

    library::~library()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void
    library::process_events(const SDL_Event* event)
    {
        ImGui_ImplSDL2_ProcessEvent(event);
    }

    void
    library::start_new_frame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window_);
        ImGui::NewFrame();
    }

    void
    library::render()
    {
        // ImGui::ShowTestWindow();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

}
