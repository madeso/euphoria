#include "t3d/editor.h"


#include "core/intersection.h"

#include "render/actor.h"

#include "t3d/tool.h"
#include "t3d/tilelibrary.h"


namespace euphoria::t3d
{
    editor::editor
    (
            t3d::grid* agrid,
            render::world* aworld,
            t3d::tile_library* atile_library
    )
        : grid(agrid)
        , world(aworld)
        , tile_library(atile_library)
        , camera(core::mat4f::identity(), core::mat4f::identity())
        , viewport(core::Recti::from_width_height(10, 10))
        , mouse(core::Vec2i {0, 0})
    {
    }


    std::shared_ptr<placed_mesh>
    editor::get_first_selected_or_null()
    {
        for(auto act : actors)
        {
            if(act->is_selected)
            {
                return act;
            }
        }

        return nullptr;
    }


    void
    editor::set_all_selected(bool is_selected)
    {
        for(auto actor: actors)
        {
            actor->is_selected = is_selected;
        }
    }


    std::vector<std::shared_ptr<placed_mesh>>
    editor::raycast(const core::UnitRay3f& ray)
    {
        std::vector<std::shared_ptr<placed_mesh>> r;

        for(auto a: actors)
        {
            const auto collision = core::get_intersection(ray, a->tile->aabb.offset_copy(a->actor->position));
            
            if(collision.intersected)
            {
                r.emplace_back(a);
            }
        }

        return r;
    }


    // current tool callbacks
    bool
    editor::is_busy()
    {
        return tools.get_current_tool()->is_busy(this);
    }


    void
    editor::step()
    {
        tools.get_current_tool()->step(this);
    }


    void
    editor::on_mouse(core::MouseButton button, bool down)
    {
        tools.get_current_tool()->on_mouse(this, button, down);
    }


    void
    editor::on_key(core::Key key, bool down)
    {
        tools.get_current_tool()->on_key(this, key, down);
    }


    void
    editor::on_scroll(const core::Vec2i& scroll)
    {
        tools.get_current_tool()->on_scroll(this, scroll);
    }


    void
    editor::on_editor()
    {
        tools.get_current_tool()->on_editor(this);
    }
}
