#include "core/camera3.editor.h"

#include <iostream>

#include "core/camera3.h"
#include "core/viewport.h"
#include "core/intersection.h"
#include "core/plane.h"


namespace euphoria::core
{
    /*
    todo:
        pan:
            1. raycast from cursor to a point
            2. save plane with camera normal and compiled camera
            3. one move raycast from current camera position to planeand get 3d offset from plane 0
        rotate:
            1. raycast from window center to a point, that is now origo of orientation
            2. orbit orientation is now reverse
        zoom:
            > reminder that you should use exponential interpolation, not lerp, for multiplicative quantities~
            > if you want to find the frequency, zoom level or scale, halfway between 2 and 8, then the right answer is 4, not 5
            > lerp( 2, 8, 0.5 ) = 5
            > eerp( 2, 8, 0.5 ) = 4
            https://mobile.twitter.com/FreyaHolmer/status/1486714301540831234?t=pA5lC-hN6l_AssTX-rxKbw&s=09
        saved viewports/camera spaces, # button to goto, ctrl+# to save
        lerp to new camera position over time
        focus on object
        2d camera
    */
    namespace detail
    {
        void
        update_state(EditorCamera3* self)
        {
            if(self->next_state != nullptr)
            {
                self->state = std::move(self->next_state);
            }
        }


        EditorCameraStyle3
        next_style(EditorCameraStyle3 current_style)
        {
            switch(current_style)
            {
                default:
                case EditorCameraStyle3::fps: return EditorCameraStyle3::orbital;
                case EditorCameraStyle3::orbital: return EditorCameraStyle3::fps;
            }
        }


        ///////////////////////////////////////////////////////////////////////////////////////////////

        // represents a stored position and rotation of the editor camera
        struct CameraFrame
        {
            Angle rotation_angle;
            Angle look_angle;
            Vec3f position;
        };

        CameraFrame frame_from_editor(EditorCamera3* editor)
        {
            return
            {
                editor->fps.rotation_angle,
                editor->fps.look_angle,
                editor->fps.position
            };
        }

        void frame_to_editor(const CameraFrame& frame, EditorCamera3* editor)
        {
            editor->fps.rotation_angle = frame.rotation_angle;
            editor->fps.look_angle = frame.look_angle;
            editor->fps.position = frame.position;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////

        
        std::unique_ptr<detail::EditorCameraState3> make_orbit_camera
        (
            const CameraFrame& start,
            const CompiledCamera3& latest_camera,
            const Viewport& latest_viewport,
            const Vec2i& latest_mouse,
            bool latest_shift
        );
        std::unique_ptr<detail::EditorCameraState3> make_default_camera();


        ///////////////////////////////////////////////////////////////////////////////////////////////

        struct OrbitCamera : EditorCameraState3
        {
            CameraFrame start_frame;
            CompiledCamera3 camera;
            Viewport viewport;
            Vec2i start_mouse;
            Vec2i mouse;
            bool shift;

            struct PanData
            {
                std::optional<Vec3f> center;
                std::optional<Plane> plane;

                PanData
                (
                    std::optional<Vec3f> collision,
                    const Unit3f& normal
                )
                : center(collision)
                {
                    if(collision)
                    {
                        plane = Plane::from_normal_and_point(normal, *collision);
                    }
                }
            };

            struct OrbitData
            {
            };
            
            std::optional<PanData> pan;
            std::optional<OrbitData> orbit;

            OrbitCamera
            (
                // need mouse center
                const CameraFrame& start,
                const CompiledCamera3& latest_camera,
                const Viewport& latest_viewport,
                const Vec2i& latest_mouse,
                bool latest_shift
            )
                : start_frame(start)
                , camera(latest_camera)
                , viewport(latest_viewport)
                , start_mouse(latest_mouse)
                , mouse(latest_mouse)
                , shift(latest_shift)
            {
                std::cout << "start mouse " << start_mouse.x << ", " << start_mouse.y << "\n";
            }

            [[nodiscard]]
            bool
            is_panning() const
            {
                return shift;
            }

            void
            update_data(EditorCamera3* owner)
            {
                if(is_panning())
                {
                    orbit.reset();
                    if(pan.has_value() == false)
                    {
                        pan = PanData
                        {
                            owner->raycast(start_mouse, camera, viewport),
                            camera.view.get_z_axis().get_normalized()
                        };
                    }
                }
                else
                {
                    pan.reset();
                    if(orbit.has_value() == false)
                    {
                        orbit = OrbitData
                        {
                        };
                    }
                }
            }

            void
            update_camera(EditorCamera3* owner)
            {
                if(is_panning())
                {
                    if(pan.has_value() == false) { return; }
                    if(pan->plane.has_value() == false) { return; }
                    const auto ray = mouse_to_unit_ray(camera, viewport, mouse);
                    std::cout << "ray: " << ray.dir << "\n";
                    const auto distance = get_intersection(ray, *pan->plane);
                    std::cout << "dist: " << distance << "\n";
                    const auto new_center = ray.get_point(distance);
                    const auto offset = Vec3f::from_to(*pan->center, new_center);
                    std::cout << "3d offset " << *pan->center << "\n   -> " << new_center << "\n   = " << offset << ": " << offset.get_length() << "\n";
                    auto new_frame = start_frame;
                    new_frame.position += offset;
                    detail::frame_to_editor(new_frame, owner);
                }
                // todo(Gustav): implement rotate
            }

            void
            on_mouse_move(EditorCamera3* owner, int dx, int dy) override
            {
                update_data(owner);
                std::cout << "mouse " << mouse.x << ", " << mouse.y << "\n";
                std::cout << "mouse move " << dx << ", " << dy << "\n";
                mouse.x += dx;
                mouse.y += dy;
                update_camera(owner);
            }

            void
            on_key(EditorCamera3*, Key, bool) override
            {
            }

            void
            step
            (
                EditorCamera3* owner,
                bool shift_state,
                const Vec2i&,
                const CompiledCamera3&,
                const Viewport&,
                float
            ) override
            {
                if(shift_state != shift)
                {
                    shift = shift_state;
                    update_data(owner);
                    update_camera(owner);
                }
            }

            void
            on_camera_start(EditorCamera3*) override
            {
            }

            void
            on_camera_stop(EditorCamera3* owner) override
            {
                owner->next_state = make_default_camera();
            }

            MouseBehaviour
            get_mouse(EditorCamera3*) override
            {
                return MouseBehaviour::wrap_within_window;
            }
        };


        ///////////////////////////////////////////////////////////////////////////////////////////////


        struct DefaultCamera : EditorCameraState3
        {
            bool latest_shift = false;
            Vec2i latest_mouse = {0,0};
            CompiledCamera3 latest_camera;
            Viewport latest_viewport;
            bool looking = false;

            DefaultCamera()
                : latest_camera
                (
                    Mat4f::identity(),
                    Mat4f::identity()
                )
                , latest_viewport
                (
                    Recti::from_width_height(0, 0)
                )
            {
            }


            void
            on_mouse_move(EditorCamera3* owner, int dx, int dy) override
            {
                if(owner->style == EditorCameraStyle3::fps)
                {
                    owner->fps.look(core::c_int_to_float(dx), core::c_int_to_float(dy));
                }
            }


            void
            on_key(EditorCamera3* owner, Key key, bool down) override
            {
                if(owner->style == EditorCameraStyle3::fps)
                {
                    owner->fps.on_key(key, down);
                }
            }


            void
            step
            (
                EditorCamera3* owner,
                bool shift,
                const Vec2i& mouse,
                const CompiledCamera3& camera,
                const Viewport& viewport,
                float dt
            ) override
            {
                latest_shift = shift;
                latest_mouse = mouse;
                latest_camera = camera;
                latest_viewport = viewport;
                if(owner->style == EditorCameraStyle3::fps)
                {
                    owner->fps.update(dt);
                }
            }


            void
            on_camera_start(EditorCamera3* owner) override
            {
                if(owner->style == EditorCameraStyle3::orbital)
                {
                    owner->next_state = make_orbit_camera
                    (
                        detail::frame_from_editor(owner),
                        latest_camera,
                        latest_viewport,
                        latest_mouse,
                        latest_shift
                    );
                    looking = true;
                }
            }


            void
            on_camera_stop(EditorCamera3*) override
            {
                looking = false;
            }

            MouseBehaviour
            get_mouse(EditorCamera3*) override
            {
                if(looking)
                {
                    return MouseBehaviour::relative_only;
                }
                else
                {
                    return MouseBehaviour::normal;
                }
            }
        };


        void
        set_default_state(EditorCamera3* cam)
        {
            cam->state = make_default_camera();
        }
    


        ///////////////////////////////////////////////////////////////////////////////////////////

        
        std::unique_ptr<detail::EditorCameraState3> make_orbit_camera
        (
            const detail::CameraFrame& start,
            const CompiledCamera3& latest_camera,
            const Viewport& latest_viewport,
            const Vec2i& latest_mouse,
            bool latest_shift
        )
        {
            return std::make_unique<detail::OrbitCamera>(start, latest_camera, latest_viewport, latest_mouse, latest_shift);
        }


        std::unique_ptr<detail::EditorCameraState3> make_default_camera()
        {
            return std::make_unique<detail::DefaultCamera>();
        }

    }


    ///////////////////////////////////////////////////////////////////////////////////////////////


    EditorCamera3::EditorCamera3()
    {
        // todo(Gustav): default to inverted? or don't subtract delta values
        fps.look_sensitivity.inverted = true;
        fps.look_angle = Angle::from_degrees(-30.0f);
        detail::set_default_state(this);
    }


    void
    EditorCamera3::step
    (
        bool shift_state,
        const Vec2i& mouse,
        const CompiledCamera3& camera,
        const Viewport& viewport,
        float dt
    )
    {
        if(state == nullptr) { return; }
        state->step(this, shift_state, mouse, camera, viewport, dt);
        detail::update_state(this);
    }


    void
    EditorCamera3::on_mouse_move(int dx, int dy)
    {
        if(state == nullptr) { return; }
        state->on_mouse_move(this, dx, dy);
        detail::update_state(this);
    }


    void
    EditorCamera3::on_key(Key key, bool down)
    {
        if(state == nullptr) { return; }
        state->on_key(this, key, down);
        detail::update_state(this);
    }


    void
    EditorCamera3::on_camera_start()
    {
        if(state == nullptr) { return; }
        state->on_camera_start(this);
        detail::update_state(this);
    }


    void
    EditorCamera3::on_camera_stop()
    {
        fps.is_left_down = false;
        fps.is_right_down = false;
        fps.is_forward_down = false;
        fps.is_backward_down = false;
        fps.is_up_down = false;
        fps.is_down_down = false;

        if(state == nullptr) { return; }
        state->on_camera_stop(this);
        detail::update_state(this);
    }


    bool
    EditorCamera3::is_camera_orbit() const
    {
        return style == EditorCameraStyle3::orbital;
    }


    void
    EditorCamera3::toggle_camera_orbit()
    {
        style = detail::next_style(style);
    }

    MouseBehaviour
    EditorCamera3::get_mouse()
    {
        if(state == nullptr) { return MouseBehaviour::normal; }
        return state->get_mouse(this);
    }
}
