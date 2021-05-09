#include "render/scalablesprite.h"

#include <iostream>
#include <algorithm>

#include "core/assert.h"
#include "core/rect.h"
#include "core/proto.h"
#include "core/tablelayout.h"
// #include "core/bufferbuilder2d.h"

#include "render/buffer2d.h"
#include "render/texture.h"
#include "render/texturecache.h"
#include "render/spriterender.h"

#include "gaf_scalingsprite.h"


using namespace euphoria::convert;

////////////////////////////////////////////////////////////////////////////////

namespace
{
    float
    CopyData(std::vector<float>* dest, const std::vector<int>& src)
    {
        dest->reserve(src.size());
        float size = 0;
        for(const int s: src)
        {
            const auto f = static_cast<float>(s);
            dest->push_back(f);
            size += euphoria::core::abs(f);
        }

        if(dest->empty())
        {
            dest->push_back(-100.0f);
            return 100.0f;
        }

        return size;
    }


    float
    GetConstantSize(const std::vector<float>& data)
    {
        float r = 0;
        for(float f: data)
        {
            if(f > 0)
            {
                r += f;
            }
        }
        return r;
    }
}


namespace euphoria::render
{
    ScalableSprite::ScalableSprite
    (
        core::vfs::FileSystem* fs,
        const core::vfs::FilePath& path,
        TextureCache* cache
    )
        : texture_(cache->GetTexture(path))
    {
        scalingsprite::ScalingSprite sprite;

        core::read_json_to_gaf_struct_or_get_error_message
        (
            fs,
            &sprite,
            path.SetExtensionCopy(path.GetExtension()+ ".json")
        );

        max_row_ = CopyData(&rows_, sprite.rows);
        max_col_ = CopyData(&cols_, sprite.cols);
    }


    ScalableSprite::~ScalableSprite() = default;


    core::Sizef
    ScalableSprite::GetMinimumSize() const
    {
        return core::Sizef::create_from_width_height
        (
            GetConstantSize(cols_),
            GetConstantSize(rows_)
        );
    }


    void
    ScalableSprite::Render(SpriteRenderer* sr, const core::rectf& rect, const core::rgba& tint
    ) const
    {
        const auto size_ = rect.get_size();
        const auto pos = rect.get_bottom_left();
        const auto position_cols = core::perform_table_layout(cols_, size_.width);
        const auto position_rows = core::perform_table_layout
        (
            rows_,
            size_.height
        );

        const auto cols_size = cols_.size();
        const auto rows_size = rows_.size();

        ASSERT(position_rows.size() == rows_size);
        ASSERT(position_cols.size() == cols_size);

        float position_current_col = 0;
        float uv_current_col = 0;
        for(unsigned int c = 0; c < cols_size; ++c)
        {
            float position_current_row = size_.height;
            float uv_current_row = 1;

            const auto position_next_col = position_current_col + position_cols[c];
            const auto uv_next_col = uv_current_col + core::abs(cols_[c]) / max_col_;

            for(unsigned int r = 0; r < rows_size; ++r)
            {
                const auto position_next_row = position_current_row - position_rows[r];
                const auto uv_next_row = uv_current_row - core::abs(rows_[r]) / max_row_;

                ASSERTX
                (
                    position_current_row > position_next_row,
                    position_current_row,
                    position_next_row
                );
                ASSERTX
                (
                    uv_current_row > uv_next_row,
                    uv_current_row,
                    uv_next_row
                );
                const auto position_rect = core::rectf::from_left_right_top_bottom
                (
                    position_current_col,
                    position_next_col,
                    position_current_row,
                    position_next_row
                );
                const auto uv_rect = core::rectf::from_left_right_top_bottom
                (
                    uv_current_col,
                    uv_next_col,
                    uv_current_row,
                    uv_next_row
                );

                sr->DrawRect
                (
                    *texture_,
                    position_rect.offset_copy(pos),
                    uv_rect,
                    0.0_rad,
                    core::scale2f {0, 0},
                    tint
                );

                position_current_row = position_next_row;
                uv_current_row = uv_next_row;
            }
            position_current_col = position_next_col;
            uv_current_col = uv_next_col;
        }
    }
}
