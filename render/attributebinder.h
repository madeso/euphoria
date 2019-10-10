#ifndef EUPHORIA_ATTRIBUTEBINDER_H
#define EUPHORIA_ATTRIBUTEBINDER_H

#include "render/shaderattribute.h"

#include <memory>
#include <vector>

namespace euphoria::render
{
    struct ShaderAttribute;
    struct CompiledMeshPart;

    struct AttributeBinder
    {
    public:
        void
        Register(const ShaderAttribute& attribute);

        void
        Bind(const std::shared_ptr<CompiledMeshPart>& part);

    private:
        struct BindData
        {
            BindData(ShaderAttribute a, int s);

            ShaderAttribute attribute;
            int             size;
        };

        int                   total_size_ = 0;
        std::vector<BindData> bind_datas_;
    };
}  // namespace euphoria::render

#endif  // EUPHORIA_ATTRIBUTEBINDER_H
