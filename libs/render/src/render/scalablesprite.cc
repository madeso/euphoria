#include "render/scalablesprite.h"

#include <iostream>
#include <algorithm>

#include "assert/assert.h"
#include "core/rect.h"
#include "core/proto.h"
#include "core/tablelayout.h"
// #include "core/bufferbuilder2d.h"

#include "render/buffer2d.h"
#include "render/texture.h"
#include "render/texturecache.h"
#include "render/spriterender.h"

#include "gaf_scalingsprite.h"
#include "gaf_rapidjson_scalingsprite.h"

using namespace euphoria::convert;

////////////////////////////////////////////////////////////////////////////////

namespace
{
    float
    copy_data(std::vector<float>* dest, const std::vector<int>& src)
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
    get_constant_size(const std::vector<float>& data)
    {
        float ret = 0;
        for(float f: data)
        {
            if(f > 0)
            {
                ret += f;
            }
        }
        return ret;
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
        : texture(cache->get_texture(path))
    {
        const auto json_path = path.set_extension_copy(path.get_extension()+ ".json");
        const auto sprite = core::get_default_but_log_errors
        (
            core::read_json_file_to_gaf_struct<scalingsprite::ScalingSprite>(fs, json_path, scalingsprite::ReadJsonScalingSprite)
        );

        calculated_texture_size_rows = copy_data(&rows, sprite.rows);
        calculated_texture_size_columns = copy_data(&columns, sprite.cols);
    }


    ScalableSprite::~ScalableSprite() = default;


    core::size2f
    ScalableSprite::get_minimum_size() const
    {
        return core::size2f::create_from_width_height
        (
            get_constant_size(columns),
            get_constant_size(rows)
        );
    }


    void
    ScalableSprite::render(SpriteRenderer* renderer, const core::Rectf& rect, const core::rgba& tint) const
    {
        const auto size = rect.get_size();
        const auto pos = rect.get_bottom_left();
        const auto position_cols = core::perform_table_layout(columns, size.width);
        const auto position_rows = core::perform_table_layout(rows, size.height);

        const auto cols_size = columns.size();
        const auto rows_size = rows.size();

        ASSERT(position_rows.size() == rows_size);
        ASSERT(position_cols.size() == cols_size);

        float position_current_col = 0;
        float uv_current_col = 0;
        for(unsigned int column_index = 0; column_index < cols_size; ++column_index)
        {
            float position_current_row = size.height;
            float uv_current_row = 1;

            const auto position_next_col = position_current_col + position_cols[column_index];
            const auto uv_next_col = uv_current_col + core::abs(columns[column_index]) / calculated_texture_size_columns;

            for(unsigned int row_index = 0; row_index < rows_size; ++row_index)
            {
                const auto position_next_row = position_current_row - position_rows[row_index];
                const auto uv_next_row = uv_current_row - core::abs(rows[row_index]) / calculated_texture_size_rows;

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
                const auto position_rect = core::Rectf::from_left_right_top_bottom
                (
                    position_current_col,
                    position_next_col,
                    position_current_row,
                    position_next_row
                );
                const auto uv_rect = core::Rectf::from_left_right_top_bottom
                (
                    uv_current_col,
                    uv_next_col,
                    uv_current_row,
                    uv_next_row
                );

                renderer->draw_rect
                (
                    *texture,
                    position_rect.translate_copy(pos),
                    uv_rect,
                    0.0_rad,
                    core::Scale2f{0, 0},
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
