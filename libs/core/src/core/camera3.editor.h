#pragma once

#include <memory>
#include <optional>


#include "core/fpscontroller.h"
#include "core/mousebehaviour.h"
#include "core/easing.h"


namespace euphoria::core
{
    struct UnitRay3f;
    struct Camera3;
    struct CompiledCamera3;
    struct Viewport;
    struct SphereAndPosition;
    
    struct EditorCamera3;

    namespace detail
    {
        // represents a stored position and rotation of the editor camera
        struct CameraFrame
        {
            Angle rotation_angle;
            Angle look_angle;
            vec3f position;
        };

        std::string to_string(const CameraFrame& cf);

        struct EditorCameraState3
        {
            EditorCameraState3() = default;
            virtual ~EditorCameraState3() = default;

            EditorCameraState3(EditorCameraState3&&) = delete;
            EditorCameraState3(const EditorCameraState3&) = delete;
            void operator=(EditorCameraState3&&) = delete;
            void operator=(const EditorCameraState3&) = delete;

            virtual void on_mouse_move(EditorCamera3* owner, int dx, int dy) = 0;
            virtual void on_key(EditorCamera3* owner, Key key, bool down) = 0;
            virtual void update
            (
                EditorCamera3* owner,
                bool shift_state,
                const vec2i& mouse,
                const CompiledCamera3& camera,
                const Viewport& viewport,
                float dt
            ) = 0;

            virtual void
            on_scroll(EditorCamera3* owner, int dx, int dy) = 0;

            virtual void on_camera_start(EditorCamera3* owner) = 0;
            virtual void on_camera_stop(EditorCamera3* owner) = 0;

            virtual MouseBehaviour get_mouse(const EditorCamera3* owner) const = 0;
        };
    }

    enum class EditorCameraStyle3
    {
        orbital, fps
    };

    struct EditorCamera3
    {
        static constexpr int max_stored_index = 10;

        EditorCameraStyle3 style = EditorCameraStyle3::orbital;
        FpsController fps;
        float zoom_percent = 0.1f;
        float max_zoom_change = 25.0f;

        bool animate_camera = true;
        float camera_lerp_time = 0.2f;
        bool scroll_in_orbit = false;
        easing::Function lerp_function = easing::Function::smooth_stop3;

        std::unique_ptr<detail::EditorCameraState3> state;
        std::unique_ptr<detail::EditorCameraState3> next_state;
        std::vector<detail::CameraFrame> stored_cameras;


        EditorCamera3();
        virtual ~EditorCamera3() = default;

        EditorCamera3(EditorCamera3&&) = default;
        EditorCamera3& operator=(EditorCamera3&&) = default;

        EditorCamera3(const EditorCamera3&) = delete;
        EditorCamera3& operator=(const EditorCamera3&) = delete;

        void update
        (
            bool shift_state,
            const vec2i& mouse,
            const CompiledCamera3& camera,
            const Viewport& viewport,
            float dt
        );

        void on_mouse_move(int dx, int dy);
        void on_scroll(int dx, int dy);
        void on_key(Key key, bool down);
        void on_camera_start();
        void on_camera_stop();
        void toggle_camera_orbit();
        void save_camera(int id);
        void load_camera(int id);
        void focus(const SphereAndPosition& s, const Camera3& cam);
        void apply_frame(const detail::CameraFrame& frame);


        [[nodiscard]] MouseBehaviour get_mouse() const;
        [[nodiscard]] bool is_camera_orbit() const;

        virtual std::optional<vec3f> raycast (const UnitRay3f& ray) = 0;
    };
}

ADD_DEFAULT_FORMATTER(euphoria::core::detail::CameraFrame, std::string, euphoria::core::detail::to_string);
