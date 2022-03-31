#pragma once

#include <memory>
#include <optional>
#include <string>

#include "core/fpscontroller.h"
#include "core/mousebehaviour.h"


namespace euphoria::core
{
    struct CompiledCamera3;
    struct Viewport;
    
    struct EditorCamera3;

    namespace detail
    {
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
        on_key(Key key, bool down);

        void on_camera_start();
        void on_camera_stop();

        [[nodiscard]]
        bool
        is_camera_orbit() const;

        void
        toggle_camera_orbit();

        virtual
        std::optional<Vec3f>
        raycast
        (
            const Vec2i& mouse,
            const CompiledCamera3& camera,
            const Viewport& viewport
        ) = 0;
        
        EditorCameraStyle3 style = EditorCameraStyle3::orbital;
        FpsController fps;
        std::unique_ptr<detail::EditorCameraState3> state;
        std::unique_ptr<detail::EditorCameraState3> next_state;
    };
}
