#pragma once

#include <memory>

#include "core/fpscontroller.h"

namespace euphoria::core
{
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

            virtual void on_mouse_move(EditorCamera3* owner, float dx, float dy) = 0;
            virtual void on_key(EditorCamera3* owner, Key key, bool down) = 0;
            virtual void step(EditorCamera3* owner, float dt) = 0;
        };
    }

    enum class EditorCameraStyle3
    {
        oribital, fps
    };

    struct EditorCamera3
    {
        EditorCamera3();

        void step(float dt);

        void
        look(float delta_rot, float delta_look);
        
        void
        on_key(Key key, bool down);

        void on_lost_camera();

        EditorCameraStyle3 style = EditorCameraStyle3::fps;

        FpsController fps;
        std::unique_ptr<detail::EditorCameraState3> state;

    };
}
