#include "render/attributebinder.h"

#include <numeric>

#include "render/buffer.h"

namespace eu::render
{
    void
    bind_attributes
    (
            const std::vector<ShaderAttribute>& attributes,
            PointLayout* layout
    )
    {
        const int total_size = std::accumulate
        (
            attributes.begin(),
            attributes.end(),
            0,
            [](int value, const ShaderAttribute& att) -> int
            {
                return value + att.get_byte_size();
            }
        );

        int offset = 0;
        for(const auto& attribute: attributes)
        {
            layout->bind_data(attribute, total_size, offset);
            offset += attribute.get_byte_size();
        }
    }
}
