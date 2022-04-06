#include "core/camera3.editor.h"

#include "log/log.h"

#include "core/camera3.h"
#include "core/viewport.h"
#include "core/intersection.h"
#include "core/plane.h"
#include "core/viewport.h"


namespace euphoria::core
{
    /*
    todo:
        tweak zoom factor:
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
        template<typename Stream>
        Stream& operator<<(Stream& s, const CameraFrame& f)
        {
            s   << "["
                << f.rotation_angle << " "
                << f.look_angle << " "
                << f.position
                << "]";
            return s;
        }
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

        struct PanData
        {
            std::optional<Vec3f> collision;
        };

        struct OrbitData
        {
            bool valid;
            Vec3f center;
            float distance;
            Angle rotation_angle;
            Angle look_angle;

            OrbitData(std::optional<Vec3f> collision, const CameraFrame& f)
                : valid(collision.has_value())
                , center(collision.has_value() ? *collision : Vec3f::zero())
                , distance( Vec3f::from_to(center, f.position).get_length() )
                , rotation_angle(f.rotation_angle)
                , look_angle(f.look_angle)
            {
            }
        };

        std::optional<float>
        calculate_zoom_move(int dx, int dy, float length, EditorCamera3* owner)
        {
            const auto change = dx+dy;
            if (change == 0) { return std::nullopt; }

            const auto suggested_move = c_int_to_float(change) * length * owner->zoom_percent;
            const auto sign = core::get_sign(change > 0);
            const auto val = core::min(core::square(suggested_move), length);
            return sign * core::min(val, owner->max_zoom_change);
        }
                

        struct OrbitCamera : EditorCameraState3
        {
            CameraFrame start_frame;
            CompiledCamera3 camera;
            CompiledCamera3 latest_camera;
            Viewport viewport;
            Vec2i start_mouse;
            Vec2i mouse;
            bool shift;
            
            std::optional<PanData> pan;
            std::optional<OrbitData> orbit;

            OrbitCamera
            (
                // need mouse center
                const CameraFrame& start,
                const CompiledCamera3& latest_c,
                const Viewport& latest_viewport,
                const Vec2i& latest_mouse,
                bool latest_shift
            )
                : start_frame(start)
                , camera(latest_c)
                , latest_camera(latest_c)
                , viewport(latest_viewport)
                , start_mouse(latest_mouse)
                , mouse(latest_mouse)
                , shift(latest_shift)
            {
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
                        const auto ray = mouse_to_unit_ray(camera, viewport, start_mouse);
                        pan = PanData
                        {
                            owner->raycast(ray)
                        };
                    }
                }
                else
                {
                    pan.reset();
                    if(orbit.has_value() == false)
                    {
                        const auto ray = camera.clip_to_world_ray(Vec2f{0.0f, 0.0f}).get_normalized();
                        orbit = OrbitData
                        {
                            owner->raycast(ray),
                            start_frame
                        };
                    }
                }
            }

            Vec3f get_far_point(const Vec2i& p, const CompiledCamera3& cc)
            {
                return mouse_to_ray(cc, viewport, p).get_point(1.0f);
            }

            void
            update_panning(EditorCamera3* owner)
            {
                if(pan.has_value() == false) { return; }
                if(pan->collision.has_value() == false) { return; }

                // based on https://prideout.net/blog/perfect_panning/

                const auto far_start = get_far_point(start_mouse, camera);
                const auto u = Vec3f::from_to(start_frame.position, *pan->collision).get_length();
                const auto v = Vec3f::from_to(*pan->collision, far_start).get_length();

                const auto far_point = get_far_point(mouse, latest_camera);
                const auto offset = Vec3f::from_to(far_start, far_point) * (-u / v);
                
                auto new_frame = start_frame;
                new_frame.position += offset;
                detail::frame_to_editor(new_frame, owner);
            }

            void
            update_orbit(EditorCamera3* owner)
            {
                if(orbit.has_value() == false) { return; }
                if(orbit->valid == false) { return; }

                const auto out = FpsController::calculate_rotation
                (
                    orbit->rotation_angle, orbit->look_angle
                ).create_from_right_up_in(Vec3f{0.0f, 0.0f, -1.0f});

                const auto new_frame = CameraFrame
                {
                    orbit->rotation_angle,
                    orbit->look_angle,
                    orbit->center + orbit->distance * out
                };
                detail::frame_to_editor(new_frame, owner);
            }

            void
            update_camera(EditorCamera3* owner)
            {
                if(is_panning())
                {
                    update_panning(owner);
                }
                else
                {
                    update_orbit(owner);
                }
            }

            void
            on_mouse_move(EditorCamera3* owner, int dx, int dy) override
            {
                ASSERT(owner != nullptr);

                if(orbit.has_value() && orbit->valid)
                {
                    const auto x = core::c_int_to_float(dx);
                    const auto y = core::c_int_to_float(dy);
                    orbit->rotation_angle += Angle::from_degrees(-x * owner->fps.look_sensitivity.get_multiplier_with_sign());
                    orbit->look_angle += Angle::from_degrees(-y * owner->fps.look_sensitivity.get_multiplier_with_sign());
                }
            }

            void
            on_scroll(EditorCamera3* owner, int dx, int dy) override
            {
                // only orbit can zoom
                if(orbit.has_value() && orbit->valid )
                {
                    const auto move = calculate_zoom_move(dx, dy, orbit->distance, owner); 
                    if(move)
                    {
                        orbit->distance += *move;
                    }
                }
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
                const Vec2i& m,
                const CompiledCamera3& cc,
                const Viewport&,
                float
            ) override
            {
                latest_camera = cc;
                bool changed = false;

                if(shift_state != shift)
                {
                    shift = shift_state;
                    changed = true;
                }

                if(mouse != m)
                {
                    mouse = m;
                    changed = true;
                }

                if(changed)
                {
                    update_data(owner);
                    mouse = m;
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
            on_scroll(EditorCamera3* owner, int dx, int dy) override
            {
                // fps camera can't scroll
                if(owner->style == EditorCameraStyle3::fps) { return; }

                const auto ray = mouse_to_unit_ray(latest_camera, latest_viewport, latest_mouse);
                const auto collision = owner->raycast(ray);

                // todo(Gustav): implement basic zoom if no collision
                if(collision.has_value() == false) { return; }

                const auto dir = Vec3f::from_to(owner->fps.position, *collision);
                const auto length = dir.get_length();
                const auto unit = dir.get_normalized();

                const auto move = calculate_zoom_move(dx, dy, length, owner);

                if(move)
                {
                    const auto change = *move * unit;
                    owner->fps.position += change;
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

        const auto default_frame = detail::frame_from_editor(this);
        for(int i=0; i<EditorCamera3::max_stored_index; i+=1)
        {
            stored_cameras.emplace_back(default_frame);
        }
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
    EditorCamera3::on_scroll(int dx, int dy)
    {
        if(state == nullptr) { return; }
        state->on_scroll(this, dx, dy);
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

    void
    EditorCamera3::save_camera(int id)
    {
        ASSERTX(id < EditorCamera3::max_stored_index, id, EditorCamera3::max_stored_index);
        const auto index = c_int_to_sizet(id);
        const auto frame = detail::frame_from_editor(this);
        stored_cameras[index] = frame;
        // LOG_INFO("Saved frame {} to {}", id, frame);
    }

    void
    EditorCamera3::load_camera(int id)
    {
        ASSERTX(id < EditorCamera3::max_stored_index, id, EditorCamera3::max_stored_index);
        const auto index = c_int_to_sizet(id);
        const auto& frame = stored_cameras[index];
        detail::frame_to_editor(frame, this);
        // LOG_INFO("Restored frame {} to {}", id, frame);
    }

    MouseBehaviour
    EditorCamera3::get_mouse()
    {
        if(state == nullptr) { return MouseBehaviour::normal; }
        return state->get_mouse(this);
    }
}
