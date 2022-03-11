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

            if(a) r = r | ao;
            if(b) r = r | bo;
            if(c) r = r | co;

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
        const std::optional<core::Vec3f>& snap,
        const core::Mat4f& camera_view,
        const core::Mat4f& camera_projection,
        const core::Mat4f& model,
        bool tx, bool ty, bool tz,
        core::Vec3f* new_position
    )
    {
        const auto mCurrentGizmoMode = is_local ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

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


    bool
    rotate
    (
        bool is_local,
        const std::optional<core::Angle>& snap,
        const core::Mat4f& camera_view,
        const core::Mat4f& camera_projection,
        const core::Mat4f& model,
        bool rx, bool ry, bool rz,
        core::Quatf* new_rotation
    )
    {
        const auto mCurrentGizmoMode = is_local ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

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

        const auto snap_degrees = snap.value_or(core::Angle::zero()).in_degrees();

        const auto was_modified = ImGuizmo::Manipulate
        (
            camera_view.get_column_major(),
            camera_projection.get_column_major(),
            *operation,
            mCurrentGizmoMode,
            model_to_modify.get_column_major(),
            nullptr,
            snap.has_value() ? &snap_degrees : nullptr
        );

        if (was_modified)
        {
            float matrixPosition[3], matrixRotation[3], matrixScale[3];
            ImGuizmo::DecomposeMatrixToComponents
            (
                model_to_modify.get_column_major(),
                matrixPosition, matrixRotation, matrixScale
            );

            *new_rotation = core::Quatf::from_ypr
            (
                core::Angle::from_degrees(matrixRotation[2]),
                core::Angle::from_degrees(matrixRotation[1]),
                core::Angle::from_degrees(matrixRotation[0])
            );
        }

        return was_modified;
    }
}
