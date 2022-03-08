#include "window/imguizmo.h"

#include "imgui/imgui.h"
#include "imguizmo/ImGuizmo.h"

namespace euphoria::window::imgui::guizmo
{
    void setup()
    {
        ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    }


    void begin()
    {
        ImGuizmo::BeginFrame();
    }


    bool
    run
    (
        bool is_local,
        const std::optional<core::Vec3f>& snap,
        const core::Mat4f& camera_view,
        const core::Mat4f& camera_projection,
        const core::Mat4f& model,
        core::Vec3f* new_position
    )
    {
        const auto mCurrentGizmoMode = is_local ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

        {
            ImGuiIO& io = ImGui::GetIO();
            ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        }

        auto model_to_modify = model;

        const auto was_modified = ImGuizmo::Manipulate
        (
            camera_view.get_column_major(),
            camera_projection.get_column_major(),
            ImGuizmo::TRANSLATE,
            mCurrentGizmoMode,
            model_to_modify.get_column_major(),
            nullptr,
            snap.has_value() ? &snap->x : nullptr
        );

        if (was_modified)
        {
            float matrixRotation[3], matrixScale[3];
            ImGuizmo::DecomposeMatrixToComponents
            (
                model_to_modify.get_column_major(),
                new_position->get_data_ptr(), matrixRotation, matrixScale
            );


        }

        return was_modified;
    }
}
