#include "core/pack.h"

#include "core/cint.h"

#include "stb_rect_pack.h"

#ifndef STBRP_LARGE_RECTS
#error int is not available in stb rect packing
#endif


namespace eu::core
{
    std::vector<std::optional<Recti>>
    pack
    (
        const size2i& container,
        const std::vector<size2i>& to_pack
    )
    {
        //init data
        const int num_rects = c_sizet_to_int(to_pack.size());
        auto packed_rects = std::vector<stbrp_rect>(num_rects);
        {
            int index = 0;
            for(const auto s: to_pack)
            {
                auto& r = packed_rects[index];

                r.id = index;
                r.w = s.width;
                r.h = s.height;

                index +=1;
            }
        }

        // call stb pack
        // documentation:
        // to guarantee best results: make sure 'num_nodes' >= 'width'
        const int num_nodes = container.width;
        auto context = stbrp_context{};
        auto nodes = std::vector<stbrp_node>(num_nodes);
        stbrp_init_target
        (
            &context,
            container.width,
            container.height,
            nodes.data(),
            num_nodes
        );
        stbrp_pack_rects(&context, packed_rects.data(), num_rects);

        // get and return data
        auto ret = std::vector<std::optional<Recti>>(to_pack.size());
        for(int rect_index = 0; rect_index < num_rects; ++rect_index)
        {
            const stbrp_rect& rect = packed_rects[rect_index];
            if(rect.was_packed == 0)
            {
                continue;
            }
            ret[rect_index] = Recti::from_top_left_width_height
            (
                core::vec2i{rect.x, container.height - rect.y - 1},
                rect.w,
                rect.h
            );
        }

        return ret;
    }
}

