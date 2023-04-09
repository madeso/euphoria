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

    namespace
    {
        std::optional<ImGuizmo::OPERATION>
        combine
        (
            bool a, ImGuizmo::OPERATION ao,
            bool b, ImGuizmo::OPERATION bo,
            bool c, ImGuizmo::OPERATION co
        )
        {
            // todo(Gustav): add a none value to ImGuizmo so we can remove the static cast
            constexpr const auto none = static_cast<ImGuizmo::OPERATION>(0);

            auto r = none;

            if(a) { r = r | ao; }
            if(b) { r = r | bo; }
            if(c) { r = r | co; }

            if (r != none)
            {
                return r;
            }
            else
            {
                return std::nullopt;
            }
        }
    }


    bool
    translate
    (
        bool is_local,
        const std::optional<core::vec3f>& snap,
        const core::mat4f& camera_view,
        const core::mat4f& camera_projection,
        const core::mat4f& model,
        bool tx, bool ty, bool tz,
        core::vec3f* new_position
    )
    {
        const auto current_gizmo_mode = is_local ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

        const auto operation = combine
        (
            tx, ImGuizmo::TRANSLATE_X,
            ty, ImGuizmo::TRANSLATE_Y,
            tz, ImGuizmo::TRANSLATE_Z
        );

        if (operation.has_value() == false)
        {
            return false;
        }

        {
            ImGuiIO& io = ImGui::GetIO();
            ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        }

        auto model_to_modify = model;

        const auto was_modified = ImGuizmo::Manipulate
        (
            camera_view.get_column_major(),
            camera_projection.get_column_major(),
            *operation,
            current_gizmo_mode,
            model_to_modify.get_column_major(),
            nullptr,
            snap.has_value() ? &snap->x : nullptr
        );

        if (was_modified)
        {
            float matrix_rotation[3];
            float matrix_scale[3];

            ImGuizmo::DecomposeMatrixToComponents
            (
                model_to_modify.get_column_major(),
                new_position->get_data_ptr(), matrix_rotation, matrix_scale
            );
        }

        return was_modified;
    }


    bool
    rotate
    (
        bool is_local,
        const std::optional<core::Angle>& snap,
        const core::mat4f& camera_view,
        const core::mat4f& camera_projection,
        const core::mat4f& model,
        bool rx, bool ry, bool rz,
        core::quatf* new_rotation
    )
    {
        const auto current_gizmo_mode = is_local ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

        const auto operation = combine
        (
            rx, ImGuizmo::ROTATE_X,
            ry, ImGuizmo::ROTATE_Y,
            rz, ImGuizmo::ROTATE_Z
        );

        if (operation.has_value() == false)
        {
            return false;
        }

        {
            ImGuiIO& io = ImGui::GetIO();
            ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        }

        auto model_to_modify = model;

        const auto snap_degrees = snap.value_or(core::no_rotation).as_degrees();

        const auto was_modified = ImGuizmo::Manipulate
        (
            camera_view.get_column_major(),
            camera_projection.get_column_major(),
            *operation,
            current_gizmo_mode,
            model_to_modify.get_column_major(),
            nullptr,
            snap.has_value() ? &snap_degrees : nullptr
        );

        if (was_modified)
        {
            float matrix_position[3];
            float matrix_rotation[3];
            float matrix_scale[3];

            ImGuizmo::DecomposeMatrixToComponents
            (
                model_to_modify.get_column_major(),
                matrix_position, matrix_rotation, matrix_scale
            );

            *new_rotation = core::quatf::from_ypr
            (
                core::Angle::from_degrees(matrix_rotation[2]),
                core::Angle::from_degrees(matrix_rotation[1]),
                core::Angle::from_degrees(matrix_rotation[0])
            );
        }

        return was_modified;
    }
}
