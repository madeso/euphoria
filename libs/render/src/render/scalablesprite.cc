#include "render/scalablesprite.h"

#include <iostream>
#include <algorithm>

#include "assert/assert.h"
#include "base/rect.h"
#include "core/tablelayout.h"

#include "files/scalingsprite.h"

#include "render/buffer2d.h"
#include "render/texture.h"
#include "render/texturecache.h"
#include "render/spriterender.h"


using namespace eu::convert;

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
            dest->emplace_back(f);
            size += eu::abs(f);
        }

        if(dest->empty())
        {
            dest->emplace_back(-100.0f);
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


namespace eu::render
{
    ScalableSprite::ScalableSprite
    (
        io::FileSystem* fs,
        const io::FilePath& path,
        TextureCache* cache
    )
        : texture(cache->get_texture(path))
    {
        const auto json_path = path.set_extension_copy(path.get_extension()+ ".json");

        files::scalingsprite::ScalingSprite sprite;
        if (const auto loaded = io::read_json_file(fs, json_path); loaded == false
            && loaded.get_error().error == io::JsonErrorType::parse_error)
        {
            LOG_ERROR("Failed to load {}: {}", json_path, loaded.get_error().display);
        }
        else if (loaded)
        {
            const auto& json = loaded.get_value();
            const auto parsed = files::scalingsprite::parse(log::get_global_logger(), &sprite, json.root, &json.doc);
            if (!parsed)
            {
                sprite = {};
            }
        }

        calculated_texture_size_rows = copy_data(&rows, sprite.rows);
        calculated_texture_size_columns = copy_data(&columns, sprite.cols);
    }


    ScalableSprite::~ScalableSprite() = default;


    size2f
    ScalableSprite::get_minimum_size() const
    {
        return size2f::create_from_width_height
        (
            get_constant_size(columns),
            get_constant_size(rows)
        );
    }


    void
    ScalableSprite::render(SpriteRenderer* renderer, const Rectf& rect, const Rgba& tint) const
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
            const auto uv_next_col = uv_current_col + abs(columns[column_index]) / calculated_texture_size_columns;

            for(unsigned int row_index = 0; row_index < rows_size; ++row_index)
            {
                const auto position_next_row = position_current_row - position_rows[row_index];
                const auto uv_next_row = uv_current_row - abs(rows[row_index]) / calculated_texture_size_rows;

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
                const auto position_rect = Rectf::from_left_right_top_bottom
                (
                    position_current_col,
                    position_next_col,
                    position_current_row,
                    position_next_row
                );
                const auto uv_rect = Rectf::from_left_right_top_bottom
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
                    Scale2f{0, 0},
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
