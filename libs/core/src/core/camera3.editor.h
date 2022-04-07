#pragma once

#include <memory>
#include <optional>
#include <string>

#include "core/fpscontroller.h"
#include "core/mousebehaviour.h"


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
            Vec3f position;
        };

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
            virtual void step
            (
                EditorCamera3* owner,
                bool shift_state,
                const Vec2i& mouse,
                const CompiledCamera3& camera,
                const Viewport& viewport,
                float dt
            ) = 0;

            virtual void
            on_scroll(EditorCamera3* owner, int dx, int dy) = 0;

            virtual void on_camera_start(EditorCamera3* owner) = 0;
            virtual void on_camera_stop(EditorCamera3* owner) = 0;

            virtual MouseBehaviour get_mouse(EditorCamera3* owner) = 0;
        };
    }

    enum class EditorCameraStyle3
    {
        orbital, fps
    };

    struct EditorCamera3
    {
        static constexpr int max_stored_index = 10;

        EditorCamera3();
        EditorCamera3(const EditorCamera3&) = delete;
        EditorCamera3& operator=(const EditorCamera3&) = delete;
        EditorCamera3(EditorCamera3&&) = default;
        EditorCamera3& operator=(EditorCamera3&&) = default;

        virtual ~EditorCamera3() = default;

        void step
        (
            bool shift_state,
            const Vec2i& mouse,
            const CompiledCamera3& camera,
            const Viewport& viewport,
            float dt
        );

        MouseBehaviour get_mouse();

        void
        on_mouse_move(int dx, int dy);

        void
        on_scroll(int dx, int dy);
        
        void
        on_key(Key key, bool down);

        void on_camera_start();
        void on_camera_stop();

        [[nodiscard]]
        bool
        is_camera_orbit() const;

        void
        toggle_camera_orbit();

        void
        save_camera(int id);

        void
        load_camera(int id);

        void
        focus(const SphereAndPosition& s, const Camera3& cam);

        virtual
        std::optional<Vec3f>
        raycast
        (
            const UnitRay3f& ray
        ) = 0;

        void apply_frame(const detail::CameraFrame& frame);
        
        EditorCameraStyle3 style = EditorCameraStyle3::orbital;
        FpsController fps;
        float zoom_percent = 0.1f;
        float max_zoom_change = 25.0f;

        bool animate_camera = true;
        float camera_lerp_time = 0.2f;
        bool scroll_in_orbit = false;

        std::unique_ptr<detail::EditorCameraState3> state;
        std::unique_ptr<detail::EditorCameraState3> next_state;
        std::vector<detail::CameraFrame> stored_cameras;
    };
}
