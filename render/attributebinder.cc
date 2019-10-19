#include "render/attributebinder.h"

#include <numeric>

#include "render/buffer.h"

namespace euphoria::render
{
    void
    BindAttributes(const std::vector<ShaderAttribute>& attributes, PointLayout* layout)
    {
        const int total_size = std::accumulate(attributes.begin(), attributes.end(), 0, [](int value, const ShaderAttribute& att) -> int {
            return value + att.GetByteSize();
        });

        int stride = 0;
        for(const auto& attribute: attributes)
        {
            layout->BindData(attribute, total_size, stride);
            stride += attribute.GetByteSize();
        }
    }
}  // namespace euphoria::render
