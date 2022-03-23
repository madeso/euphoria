#include "core/camera3.editor.h"

#include "core/camera3.h"

namespace euphoria::core
{
    /*
    todo:
        saved viewports/camera spaces, # button to goto, ctrl+# to save
        lerp to new camera position over time
        focus on object
        pan/rotate/zoom
        2d camera
    */
    namespace detail
    {
        struct DefaultCamera : EditorCameraState3
        {
            void on_mouse_move(EditorCamera3* owner, float dx, float dy) override
            {
                if(owner->style == EditorCameraStyle3::Fps)
                {
                    owner->fps.look(dx, dy);
                }
            }

            void on_key(EditorCamera3* owner, Key key, bool down) override
            {
                if(owner->style == EditorCameraStyle3::Fps)
                {
                    owner->fps.on_key(key, down);
                }
            }

            void step(EditorCamera3* owner, float dt) override
            {
                if(owner->style == EditorCameraStyle3::Fps)
                {
                    owner->fps.update(dt);
                }
            }
        };

        void set_default_state(EditorCamera3* cam)
        {
            cam->state = std::make_unique<DefaultCamera>();
        }
    }

    EditorCamera3::EditorCamera3()
    {
        // todo(Gustav): default to inverted? or don't subtract delta values
        fps.look_sensitivity.inverted = true;
        fps.look_angle = Angle::from_degrees(-30.0f);
        detail::set_default_state(this);
    }

    void EditorCamera3::step(float dt)
    {
        if(state == nullptr) { return; }
        state->step(this, dt);
    }

    void
    EditorCamera3::look(float delta_rot, float delta_look)
    {
        if(state == nullptr) { return; }
        state->on_mouse_move(this, delta_rot, delta_look);
    }
    
    void
    EditorCamera3::on_key(Key key, bool down)
    {
        if(state == nullptr) { return; }
        state->on_key(this, key, down);
    }

    void
    EditorCamera3::on_lost_camera()
    {
        fps.is_left_down = false;
        fps.is_right_down = false;
        fps.is_forward_down = false;
        fps.is_backward_down = false;
        fps.is_up_down = false;
        fps.is_down_down = false;
    }
}
