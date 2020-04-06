#include "core/pack.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"


namespace euphoria::core
{
    std::vector<std::optional<Recti>>
    Pack
    (
        const Sizei& container,
        const std::vector<Sizei>& to_pack
    )
    {
        //init data
        const int num_rects = to_pack.size();
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
            &nodes[0],
            num_nodes
        );
        stbrp_pack_rects(&context, &packed_rects[0], num_rects);

        // get and return data
        auto ret = std::vector<std::optional<Recti>>(to_pack.size());
        for(int i = 0; i < num_rects; ++i)
        {
            const stbrp_rect& rect = packed_rects[i];
            if(rect.was_packed == 0)
            {
                continue;
            }
            ret[i] = Recti::FromTopLeftWidthHeight
            (
                core::vec2i{rect.x, container.height - rect.y},
                rect.w,
                rect.h
            );
        }

        return ret;
    }
}

